/*
 * Simple synchronous userspace interface to SPI devices
 *
 * Copyright (C) 2006 SWAPP
 *	Andrea Paterniani <a.paterniani@swapp-eng.it>
 * Copyright (C) 2007 David Brownell (simplification, cleanup)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>

#include <linux/spi/spi.h>
#include "spidev.h"
#include "bkavspi.h"
#include "control.h"
#include "ControlLens.h"
#include "CamModel.h"
#include <asm/uaccess.h>


/*
 * This supports access to SPI devices using normal userspace I/O calls.
 * Note that while traditional UNIX/POSIX I/O semantics are half duplex,
 * and often mask message boundaries, full SPI support requires full duplex
 * transfers.  There are several kinds of internal message boundaries to
 * handle chipselect management and other protocol options.
 *
 * SPI has a character major number assigned.  We allocate minor numbers
 * dynamically using a bitmask.  You must use hotplug tools, such as udev
 * (or mdev with busybox) to create and destroy the /dev/spidevB.C device
 * nodes, since there is no fixed association of minor numbers with any
 * particular SPI bus or device.
 */
#define SPIDEV_MAJOR			153	/* assigned */
#define N_SPI_MINORS			32	/* ... up to 256 */



static DECLARE_BITMAP(minors, N_SPI_MINORS);



/* Bit masks for spi_device.mode management.  Note that incorrect
 * settings for some settings can cause *lots* of trouble for other
 * devices on a shared bus:
 *
 *  - CS_HIGH ... this device will be active when it shouldn't be
 *  - 3WIRE ... when active, it won't behave as it should
 *  - NO_CS ... there will be no explicit message boundaries; this
 *	is completely incompatible with the shared bus model
 *  - READY ... transfers may proceed when they shouldn't.
 *
 * REVISIT should changing those flags be privileged?
 */
#define SPI_MODE_MASK		(SPI_CPHA | SPI_CPOL | SPI_CS_HIGH \
				| SPI_LSB_FIRST | SPI_3WIRE | SPI_LOOP \
				| SPI_NO_CS | SPI_READY)
static struct spi_ioc_transfer xfer[2];

struct spidev_data {
	dev_t			devt;
	spinlock_t		spi_lock;
	struct spi_device	*spi;
	struct list_head	device_entry;

	/* buffer is NULL unless this device is open (users > 0) */
	struct mutex		buf_lock;
	unsigned		users;
	u8			*buffer;
};
struct spidev_data	*spidev[4];

static LIST_HEAD(device_list);
static DEFINE_MUTEX(device_list_lock);
static unsigned number_device=0;
static unsigned bufsiz = 4096;
module_param(bufsiz, uint, S_IRUGO);
MODULE_PARM_DESC(bufsiz, "data bytes in biggest supported SPI message");

/*-------------------------------------------------------------------------*/

/*
 * We can't use the standard synchronous wrappers for file I/O; we
 * need to protect against async removal of the underlying spi_device.
 */
void F_Lenszoom(unsigned long speed,unsigned long iSizeStep);
int F_CloseIRCut(void);
int F_OpenIRCut(void );
void F_LensInit(unsigned char nModel);
int F_MoveIris( int fSize);
void F_GetZoomIndex(void);
void F_GetFocusIndex(void);
int F_MoveFocus(unsigned long speed, unsigned long iSizeStep);
static int spibkavdev_release(struct spidev_data *spidev);
void ReleaseFunctionA(void);
static long config_function(struct spidev_data *spidev, unsigned int cmd, unsigned long arg);
//#include "control.c"
static void spidev_complete(void *arg)
{
	complete(arg);
}

static ssize_t
spidev_sync(struct spidev_data *spidev, struct spi_message *message)
{
	DECLARE_COMPLETION_ONSTACK(done);
	int status;

	message->complete = spidev_complete;
	message->context = &done;

	spin_lock_irq(&spidev->spi_lock);
	if (spidev->spi == NULL)
		status = -ESHUTDOWN;
	else
		status = spi_async(spidev->spi, message);
	spin_unlock_irq(&spidev->spi_lock);

	if (status == 0) {
		wait_for_completion(&done);
		status = message->status;
		if (status == 0)
			status = message->actual_length;
	}
	return status;
}

static inline ssize_t
spidev_sync_write(struct spidev_data *spidev, size_t len)
{
	struct spi_transfer	t = {
			.tx_buf		= spidev->buffer,
			.len		= len,
		};
	struct spi_message	m;

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	return spidev_sync(spidev, &m);
}

static inline ssize_t
spidev_sync_read(struct spidev_data *spidev, size_t len)
{
	struct spi_transfer	t = {
			.rx_buf		= spidev->buffer,
			.len		= len,
		};
	struct spi_message	m;

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	return spidev_sync(spidev, &m);
}

/*-------------------------------------------------------------------------*/

/* Read-only message with current device setup */
static ssize_t
spidev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct spidev_data	*spidev;
	ssize_t			status = 0;

	/* chipselect only toggles at start or end of operation */
	if (count > bufsiz)
		return -EMSGSIZE;

	spidev = filp->private_data;

	mutex_lock(&spidev->buf_lock);
	status = spidev_sync_read(spidev, count);
	if (status > 0) {
		unsigned long	missing;

		missing = copy_to_user(buf, spidev->buffer, status);
		if (missing == status)
			status = -EFAULT;
		else
			status = status - missing;
	}
	mutex_unlock(&spidev->buf_lock);

	return status;
}

/* Write-only message with current device setup */
static ssize_t
spidev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	struct spidev_data	*spidev;
	ssize_t			status = 0;
	unsigned long		missing;

	/* chipselect only toggles at start or end of operation */
	if (count > bufsiz)
		return -EMSGSIZE;

	spidev = filp->private_data;

	mutex_lock(&spidev->buf_lock);
	missing = copy_from_user(spidev->buffer, buf, count);
	if (missing == 0) {
		status = spidev_sync_write(spidev, count);
	} else
		status = -EFAULT;
	mutex_unlock(&spidev->buf_lock);

	return status;
}

static int spibkavdev_message(struct spidev_data *spidev,
		struct spi_ioc_transfer *u_xfers, unsigned n_xfers)
{
	struct spi_message	msg;
	struct spi_transfer	*k_xfers;
	struct spi_transfer	*k_tmp;
	struct spi_ioc_transfer *u_tmp;
	unsigned		n, total;
	u8			*buf;
	int			status = -EFAULT;
	if (!spidev->buffer) {
			spidev->buffer = kmalloc(bufsiz, GFP_KERNEL);
			if (!spidev->buffer) {
				printk("kmalloc spi buffer fail");
				status = -ENOMEM;
		}
	}
	spi_message_init(&msg);
	k_xfers = kcalloc(n_xfers, sizeof(*k_tmp), GFP_KERNEL);
	if (k_xfers == NULL)
		return -ENOMEM;

	/* Construct spi_message, copying any tx data to bounce buffer.
	 * We walk the array of user-provided transfers, using each one
	 * to initialize a kernel version of the same transfer.
	 */
	buf = spidev->buffer;
	total = 0;
	for (n = n_xfers, k_tmp = k_xfers, u_tmp = u_xfers;
			n;
			n--, k_tmp++, u_tmp++) {
		k_tmp->len = u_tmp->len;

		total += k_tmp->len;
		if (total > bufsiz) {
			status = -EMSGSIZE;
			goto done;
		}

		if (u_tmp->rx_buf) {
			k_tmp->rx_buf = buf;
		}
		if (u_tmp->tx_buf) {
			k_tmp->tx_buf = buf;
			memcpy(buf, (const u8 *)
						(uintptr_t) u_tmp->tx_buf,
					u_tmp->len);
		}
		buf += k_tmp->len;

		k_tmp->cs_change = !!u_tmp->cs_change;
		k_tmp->bits_per_word = u_tmp->bits_per_word;
		k_tmp->delay_usecs = u_tmp->delay_usecs;
		k_tmp->speed_hz = u_tmp->speed_hz;
#ifdef VERBOSE
		dev_dbg(&spidev->spi->dev,
			"  xfer len %zd %s%s%s%dbits %u usec %uHz\n",
			u_tmp->len,
			u_tmp->rx_buf ? "rx " : "",
			u_tmp->tx_buf ? "tx " : "",
			u_tmp->cs_change ? "cs " : "",
			u_tmp->bits_per_word ? : spidev->spi->bits_per_word,
			u_tmp->delay_usecs,
			u_tmp->speed_hz ? : spidev->spi->max_speed_hz);
#endif
		spi_message_add_tail(k_tmp, &msg);
	}

	status = spidev_sync(spidev, &msg);
	if (status < 0)
		goto done;

	/* copy any rx data out of bounce buffer */
	buf = spidev->buffer;
	for (n = n_xfers, u_tmp = u_xfers; n; n--, u_tmp++) {
		if (u_tmp->rx_buf) {
			memcpy((u8 *)(uintptr_t) u_tmp->rx_buf, buf,
					u_tmp->len); 
		}
		buf += u_tmp->len;
	}
	status = total;

done:
	kfree(k_xfers);
	return status;
}
static int spidev_message(struct spidev_data *spidev,
		struct spi_ioc_transfer *u_xfers, unsigned n_xfers)
{
	struct spi_message	msg;
	struct spi_transfer	*k_xfers;
	struct spi_transfer	*k_tmp;
	struct spi_ioc_transfer *u_tmp;
	unsigned		n, total;
	u8			*buf;
	int			status = -EFAULT;

	spi_message_init(&msg);
	k_xfers = kcalloc(n_xfers, sizeof(*k_tmp), GFP_KERNEL);
	if (k_xfers == NULL)
		return -ENOMEM;

	/* Construct spi_message, copying any tx data to bounce buffer.
	 * We walk the array of user-provided transfers, using each one
	 * to initialize a kernel version of the same transfer.
	 */
	buf = spidev->buffer;
	total = 0;
	for (n = n_xfers, k_tmp = k_xfers, u_tmp = u_xfers;
			n;
			n--, k_tmp++, u_tmp++) {
		k_tmp->len = u_tmp->len;

		total += k_tmp->len;
		if (total > bufsiz) {
			status = -EMSGSIZE;
			goto done;
		}

		if (u_tmp->rx_buf) {
			k_tmp->rx_buf = buf;
			if (!access_ok(VERIFY_WRITE, (u8 __user *)
						(uintptr_t) u_tmp->rx_buf,
						u_tmp->len))
				goto done;
		}
		if (u_tmp->tx_buf) {
			k_tmp->tx_buf = buf;
			if (copy_from_user(buf, (const u8 __user *)
						(uintptr_t) u_tmp->tx_buf,
					u_tmp->len))
				goto done;
		}
		buf += k_tmp->len;

		k_tmp->cs_change = !!u_tmp->cs_change;
		k_tmp->bits_per_word = u_tmp->bits_per_word;
		k_tmp->delay_usecs = u_tmp->delay_usecs;
		k_tmp->speed_hz = u_tmp->speed_hz;
#ifdef VERBOSE
		dev_dbg(&spidev->spi->dev,
			"  xfer len %zd %s%s%s%dbits %u usec %uHz\n",
			u_tmp->len,
			u_tmp->rx_buf ? "rx " : "",
			u_tmp->tx_buf ? "tx " : "",
			u_tmp->cs_change ? "cs " : "",
			u_tmp->bits_per_word ? : spidev->spi->bits_per_word,
			u_tmp->delay_usecs,
			u_tmp->speed_hz ? : spidev->spi->max_speed_hz);
#endif
		spi_message_add_tail(k_tmp, &msg);
	}

	status = spidev_sync(spidev, &msg);
	if (status < 0)
		goto done;

	/* copy any rx data out of bounce buffer */
	buf = spidev->buffer;
	for (n = n_xfers, u_tmp = u_xfers; n; n--, u_tmp++) {
		if (u_tmp->rx_buf) {
			if (__copy_to_user((u8 __user *)
					(uintptr_t) u_tmp->rx_buf, buf,
					u_tmp->len)) {
				status = -EFAULT;
				goto done;
			}
		}
		buf += u_tmp->len;
	}
	status = total;

done:
	kfree(k_xfers);
	return status;
}

void RegWriteA(unsigned long address,char inData)
{
    unsigned char   arData[10];
    arData[0]= 0x7f & (address>>8);
    arData[1] = 0xff & (address);
    arData[2]= inData;
    xfer[0].tx_buf = (unsigned long)arData;
    xfer[0].len = 3; /* Length of  command to write*/
#if (SPI_SE == 1)
    if (config_function(spidev[SPI_CE0], SPI_IOC_MESSAGE(1),(unsigned long)xfer)<0)   {
	 dbg_tfr("can't read IC ram ");
     }
#else
    if (config_function(spidev[SPI_CE1], SPI_IOC_MESSAGE(1),(unsigned long)xfer)<0)   {
	 dbg_tfr("can't read IC ram ");
     }
#endif
}

void RegReadA(unsigned long address,unsigned char* outData)
{
    char arAddress[2];
    arAddress[0] = 0x80 | (address>>8);
    arAddress[1] = 0xff & (address);
    xfer[0].tx_buf = (unsigned long)arAddress;
    xfer[0].len =2; /* Length of  command to write*/
    xfer[1].rx_buf = (unsigned long)outData;
    xfer[1].len = 1; /* Length of Data to read */
#if (SPI_SE == 1)
    if (config_function(spidev[SPI_CE0], SPI_IOC_MESSAGE(2),(unsigned long)xfer)<0)   {
	 dbg_tfr("can't read IC ram ");
     }
#else
    if (config_function(spidev[SPI_CE1], SPI_IOC_MESSAGE(2),(unsigned long)xfer)<0)   {
	 dbg_tfr("can't read IC ram ");
     }
#endif

}

void RamWrite32A(unsigned long address,unsigned long  inData)
{
    unsigned char   arData[20];
    arData[0]= 0x7f & (address>>8);
    arData[1] = 0xff & (address);
    arData[2] = 0xFF& (inData>>24);
    arData[3] = 0xFF& (inData>>16);
    arData[4] = 0xFF& (inData>>8);
    arData[5] = 0xFF& (inData);
    xfer[0].tx_buf = (unsigned long)arData;
    xfer[0].len = 6; /* Length of  command to write*/
#if (SPI_SE == 1)
    if (config_function(spidev[SPI_CE0], SPI_IOC_MESSAGE(1),(unsigned long)xfer)<0)   {
	 dbg_tfr("can't read IC ram ");
     }
#else
    if (config_function(spidev[SPI_CE1], SPI_IOC_MESSAGE(1),(unsigned long)xfer)<0)   {
	 dbg_tfr("can't read IC ram ");
     }
#endif
}

void RamRead32A(unsigned long address,unsigned long* outData)
{
    char arAddress[2];
    unsigned char* data;
    data = (unsigned char *) kmalloc( 4*sizeof( unsigned char), GFP_KERNEL);
    arAddress[0] = 0x80 | (address>>8);
    arAddress[1] = 0xff & (address);
    xfer[0].tx_buf = (unsigned long)arAddress;
    xfer[0].len =2; /* Length of  command to write*/
    xfer[1].len = 4; /* Length of Data to read */
    xfer[1].rx_buf = (unsigned long)data;
    *outData= *(data) ;
    *outData= (*(outData)<<8)|(*data+1) ;
    *outData= (*(outData)<<8)|(*data+2) ;
    *outData= (*(outData)<<8)|(*data+3) ;
#if (SPI_SE == 1)
    if (config_function(spidev[SPI_CE0], SPI_IOC_MESSAGE(2),(unsigned long)xfer)<0)   {
	 dbg_tfr("can't read IC ram ");
     }
#else
    if (config_function(spidev[SPI_CE1], SPI_IOC_MESSAGE(2),(unsigned long)xfer)<0)   {
	 dbg_tfr("can't read IC ram ");
     }
#endif

    kfree(data);

}

int spi_config(unsigned char index){
	unsigned long mode=0;
	__u32 speed = 2000000;
	__u8 bit_mode = 8;
	if (config_function(spidev[index], SPI_IOC_WR_MODE,mode)<0)   {
		dbg_tfr( "bkav can't set spi mode \n");
		return false;
	}
	if (config_function(spidev[index], SPI_IOC_RD_MODE,mode)<0)   {
		dbg_tfr( "bkav can't set spi mode \n");
		return false;
	}
	if (config_function(spidev[index], SPI_IOC_RD_LSB_FIRST,mode)<0)   {
		dbg_tfr( "bkav can't set rd lsb first \n");
		return false;
	}

	if (config_function(spidev[index], SPI_IOC_WR_BITS_PER_WORD,bit_mode)<0)   {

		 dbg_tfr("bkav can't set WR Bits per word");
		 return false;
	}
	if (config_function(spidev[index],SPI_IOC_RD_BITS_PER_WORD ,bit_mode)<0)   {
		 dbg_tfr("bkav can't set RD Bits per word");
		 return false;
	}
	if (config_function(spidev[index], SPI_IOC_WR_MAX_SPEED_HZ,speed)<0)   {
		 dbg_tfr("bkav can't set spi wr max speed");
		 return false;
	}

	if (config_function(spidev[index], SPI_IOC_RD_MAX_SPEED_HZ,speed)<0)   {
		 dbg_tfr("bkav can't set spi RD max speed");
		 return false;
	}
	return true;
}
void F_Lenszoom(unsigned long speed,unsigned long iSizeStep ){

	MoveZoom(speed,iSizeStep);
}
EXPORT_SYMBOL_GPL(F_Lenszoom);
int F_CloseIRCut(void){
	CloseIRCut();
	return 0;
}
EXPORT_SYMBOL_GPL(F_CloseIRCut);

int F_OpenIRCut(void ){
	OpenIRCut();
	return 0;
}
EXPORT_SYMBOL_GPL(F_OpenIRCut);



int F_MoveIris( int fSize){

	MoveIris(fSize);
	return 0;
}

EXPORT_SYMBOL_GPL(F_MoveIris);

void F_LensInit(unsigned char nModel){

  	if(InitDriverLens(nModel)==0)
		printk("_____Init Finished___PP2____\n");
	else 
		printk("_____Init Error___PP2____\n");
}

void F_GetZoomIndex(void){
     GetZoomIndex();
}

EXPORT_SYMBOL_GPL(F_GetZoomIndex);

void F_GetFocusIndex(void){
	GetFocusIndex();
}

EXPORT_SYMBOL_GPL(F_GetFocusIndex);


int F_MoveFocus(unsigned long speed, unsigned long iSizeStep){
	MoveFocus(speed, iSizeStep);
	return 0;
}

EXPORT_SYMBOL_GPL(F_MoveFocus);

void ReleaseFunctionA(void){
	 spibkavdev_release(spidev[0]);
}
EXPORT_SYMBOL_GPL(ReleaseFunctionA);

static long
config_function(struct spidev_data *spidev, unsigned int cmd, unsigned long arg)
{
	int			err = 0;
	int			retval = 0;
//	struct spidev_data	*spidev;
	struct spi_device	*spi;
	u32			tmp;
	unsigned		n_ioc;
	struct spi_ioc_transfer	*ioc;

	/* Check type and command number */
	if (_IOC_TYPE(cmd) != SPI_IOC_MAGIC)
		return -ENOTTY;

	/* Check access direction once here; don't repeat below.
	 * IOC_DIR is from the user perspective, while access_ok is
	 * from the kernel perspective; so they look reversed.
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)

	if (err == 0 && _IOC_DIR(cmd) & _IOC_WRITE)
	if (err){
		printk("err access");
		return -EFAULT;

	}
	/* guard against device removal before, or while,
	 * we issue this ioctl.
	 */
//	spidev = filp->private_data;
	spin_lock_irq(&spidev->spi_lock);
	spi = spi_dev_get(spidev->spi);
	spin_unlock_irq(&spidev->spi_lock);

	if (spi == NULL)
		return -ESHUTDOWN;

	/*use the buffer lock here for triple duty:
	 *  - prevent I/O (from us) so calling spi_setup() is safe;
	 *  - prevent concurrent SPI_IOC_WR_* from morphing
	 *    data fields while SPI_IOC_RD_* reads them;
	 *  - SPI_IOC_MESSAGE needs the buffer locked "normally".
	 */
	mutex_lock(&spidev->buf_lock);

	switch (cmd) {
	/* read requests */
	case SPI_IOC_RD_MODE:
		spi->mode = arg;
		break;
	case SPI_IOC_RD_LSB_FIRST:
//		retval = __put_user((spi->mode & SPI_LSB_FIRST) ?  1 : 0,
//					(__u8 __user *)arg);
		
		break;
	case SPI_IOC_RD_BITS_PER_WORD:
	//	retval = __put_user(spi->bits_per_word, (__u8 __user *)arg);
		break;
	case SPI_IOC_RD_MAX_SPEED_HZ:
	//	retval = __put_user(spi->max_speed_hz, (__u32 __user *)arg);
	
		break;

	/* write requests */
	case SPI_IOC_WR_MODE:
		spi->mode = arg;
		dev_dbg(&spi->dev, "spi mode %02x\n", spi->mode);

		break;
	case SPI_IOC_WR_LSB_FIRST:
		retval = __get_user(tmp, (__u8 __user *)arg);
		if (retval == 0) {
			u8	save = spi->mode;

			if (tmp)
				spi->mode |= SPI_LSB_FIRST;
			else
				spi->mode &= ~SPI_LSB_FIRST;
			retval = spi_setup(spi);
			if (retval < 0)
				spi->mode = save;
			else
				dev_dbg(&spi->dev, "%csb first\n",
						tmp ? 'l' : 'm');
		}
		break;
	case SPI_IOC_WR_BITS_PER_WORD:
		spi->bits_per_word = arg;
		dev_dbg(&spi->dev, "%d bits per word\n", spi->bits_per_word);
		break;
	case SPI_IOC_WR_MAX_SPEED_HZ:

		spi->max_speed_hz = arg;
		dev_dbg(&spi->dev, "%d Hz (max)\n", spi->max_speed_hz);

		break;

	default:
		/* segmented and/or full-duplex I/O request */
		if (_IOC_NR(cmd) != _IOC_NR(SPI_IOC_MESSAGE(0))
				|| _IOC_DIR(cmd) != _IOC_WRITE) {
			retval = -ENOTTY;
			printk(" fail ENOTTY");
			break;
		}

		tmp = _IOC_SIZE(cmd);
		if ((tmp % sizeof(struct spi_ioc_transfer)) != 0) {
			retval = -EINVAL;
			printk("fail EINVAL");
			break;
		}
		n_ioc = tmp / sizeof(struct spi_ioc_transfer);
		if (n_ioc == 0)
			break;

		/* copy into scratch area */
		ioc = kmalloc(tmp, GFP_KERNEL);
		if (!ioc) {
			retval = -ENOMEM;
			printk("fail ENOMEM");
			break;
		}
		memcpy(ioc,(void *) arg, tmp);
		/* translate to spi_message, execute */
		retval = spibkavdev_message(spidev, ioc, n_ioc);
		kfree(ioc);
		kfree(spidev->buffer);
		spidev->buffer = NULL;
		break;
	}

	mutex_unlock(&spidev->buf_lock);
	spi_dev_put(spi);
	return retval;
}
static int spibkavdev_release(struct spidev_data *spidev)
{
//	struct spidev_data	*spidev;
	int			status = 0;

	mutex_lock(&device_list_lock);
//	spidev = filp->private_data;
//	filp->private_data = NULL;

	/* last close? */
	spidev->users--;
	if (!spidev->users) {
		int		dofree;

		kfree(spidev->buffer);
		spidev->buffer = NULL;

		/* ... after we unbound from the underlying device? */
		spin_lock_irq(&spidev->spi_lock);
		dofree = (spidev->spi == NULL);
		spin_unlock_irq(&spidev->spi_lock);

		if (dofree)
			kfree(spidev);
	}
	mutex_unlock(&device_list_lock);

	return status;
}

EXPORT_SYMBOL_GPL(spibkavdev_release);


static long
spidev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int			err = 0;
	int			retval = 0;
	struct spidev_data	*spidev;
	struct spi_device	*spi;
	u32			tmp;
	unsigned		n_ioc;
	struct spi_ioc_transfer	*ioc;

	/* Check type and command number */
	if (_IOC_TYPE(cmd) != SPI_IOC_MAGIC)
		return -ENOTTY;

	/* Check access direction once here; don't repeat below.
	 * IOC_DIR is from the user perspective, while access_ok is
	 * from the kernel perspective; so they look reversed.
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE,
				(void __user *)arg, _IOC_SIZE(cmd));
	if (err == 0 && _IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ,
				(void __user *)arg, _IOC_SIZE(cmd));
	if (err)
		return -EFAULT;

	/* guard against device removal before, or while,
	 * we issue this ioctl.
	 */
	spidev = filp->private_data;
	spin_lock_irq(&spidev->spi_lock);
	spi = spi_dev_get(spidev->spi);
	spin_unlock_irq(&spidev->spi_lock);

	if (spi == NULL)
		return -ESHUTDOWN;

	/* use the buffer lock here for triple duty:
	 *  - prevent I/O (from us) so calling spi_setup() is safe;
	 *  - prevent concurrent SPI_IOC_WR_* from morphing
	 *    data fields while SPI_IOC_RD_* reads them;
	 *  - SPI_IOC_MESSAGE needs the buffer locked "normally".
	 */
	mutex_lock(&spidev->buf_lock);

	switch (cmd) {
	/* read requests */
	case SPI_IOC_RD_MODE:
		retval = __put_user(spi->mode & SPI_MODE_MASK,
					(__u8 __user *)arg);
		break;
	case SPI_IOC_RD_LSB_FIRST:
		retval = __put_user((spi->mode & SPI_LSB_FIRST) ?  1 : 0,
					(__u8 __user *)arg);
		break;
	case SPI_IOC_RD_BITS_PER_WORD:
		retval = __put_user(spi->bits_per_word, (__u8 __user *)arg);
		break;
	case SPI_IOC_RD_MAX_SPEED_HZ:
		retval = __put_user(spi->max_speed_hz, (__u32 __user *)arg);
		break;

	/* write requests */
	case SPI_IOC_WR_MODE:
		retval = __get_user(tmp, (u8 __user *)arg);
		if (retval == 0) {
			u8	save = spi->mode;

			if (tmp & ~SPI_MODE_MASK) {
				retval = -EINVAL;
				break;
			}

			tmp |= spi->mode & ~SPI_MODE_MASK;
			spi->mode = (u8)tmp;

			retval = spi_setup(spi);
			if (retval < 0)
				spi->mode = save;
			else
				dev_dbg(&spi->dev, "spi mode %02x\n", tmp);
		}
		break;
	case SPI_IOC_WR_LSB_FIRST:
		retval = __get_user(tmp, (__u8 __user *)arg);
		if (retval == 0) {
			u8	save = spi->mode;

			if (tmp)
				spi->mode |= SPI_LSB_FIRST;
			else
				spi->mode &= ~SPI_LSB_FIRST;
			retval = spi_setup(spi);
			if (retval < 0)
				spi->mode = save;
			else
				dev_dbg(&spi->dev, "%csb first\n",
						tmp ? 'l' : 'm');
		}
		break;
	case SPI_IOC_WR_BITS_PER_WORD:
		retval = __get_user(tmp, (__u8 __user *)arg);
		if (retval == 0) {
			u8	save = spi->bits_per_word;

			spi->bits_per_word = tmp;
			retval = spi_setup(spi);
			if (retval < 0)
				spi->bits_per_word = save;
			else
				dev_dbg(&spi->dev, "%d bits per word\n", tmp);
		}
		break;
	case SPI_IOC_WR_MAX_SPEED_HZ:
		retval = __get_user(tmp, (__u32 __user *)arg);
		if (retval == 0) {
			u32	save = spi->max_speed_hz;

			spi->max_speed_hz = tmp;
			retval = spi_setup(spi);
			if (retval < 0)
				spi->max_speed_hz = save;
			else
				dev_dbg(&spi->dev, "%d Hz (max)\n", tmp);
		}
		break;

	default:
		/* segmented and/or full-duplex I/O request */
		if (_IOC_NR(cmd) != _IOC_NR(SPI_IOC_MESSAGE(0))
				|| _IOC_DIR(cmd) != _IOC_WRITE) {
			retval = -ENOTTY;
			break;
		}

		tmp = _IOC_SIZE(cmd);
		if ((tmp % sizeof(struct spi_ioc_transfer)) != 0) {
			retval = -EINVAL;
			break;
		}
		n_ioc = tmp / sizeof(struct spi_ioc_transfer);
		if (n_ioc == 0)
			break;

		/* copy into scratch area */
		ioc = kmalloc(tmp, GFP_KERNEL);
		if (!ioc) {
			retval = -ENOMEM;
			break;
		}
		if (__copy_from_user(ioc, (void __user *)arg, tmp)) {
			kfree(ioc);
			retval = -EFAULT;
			break;
		}

		/* translate to spi_message, execute */
		retval = spidev_message(spidev, ioc, n_ioc);
		kfree(ioc);
		break;
	}

	mutex_unlock(&spidev->buf_lock);
	spi_dev_put(spi);
	return retval;
}

#ifdef CONFIG_COMPAT
static long
spidev_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	return spidev_ioctl(filp, cmd, (unsigned long)compat_ptr(arg));
}
#else
#define spidev_compat_ioctl NULL
#endif /* CONFIG_COMPAT */

static int spidev_open(struct inode *inode, struct file *filp)
{
	struct spidev_data	*spidev;
	int			status = -ENXIO;

	mutex_lock(&device_list_lock);

	list_for_each_entry(spidev, &device_list, device_entry) {
		if (spidev->devt == inode->i_rdev) {
			status = 0;
			break;
		}
	}
	if (status == 0) {
		if (!spidev->buffer) {
			spidev->buffer = kmalloc(bufsiz, GFP_KERNEL);
			if (!spidev->buffer) {
				dev_dbg(&spidev->spi->dev, "open/ENOMEM\n");
				status = -ENOMEM;
			}
		}
		if (status == 0) {
			spidev->users++;
			filp->private_data = spidev;
			nonseekable_open(inode, filp);
		}
	} else
		pr_debug("spidev: nothing for minor %d\n", iminor(inode));

	mutex_unlock(&device_list_lock);
	return status;
}

static int spidev_release(struct inode *inode, struct file *filp)
{
	struct spidev_data	*spidev;
	int			status = 0;

	mutex_lock(&device_list_lock);
	spidev = filp->private_data;
	filp->private_data = NULL;

	/* last close? */
	spidev->users--;
	if (!spidev->users) {
		int		dofree;

		kfree(spidev->buffer);
		spidev->buffer = NULL;

		/* ... after we unbound from the underlying device? */
		spin_lock_irq(&spidev->spi_lock);
		dofree = (spidev->spi == NULL);
		spin_unlock_irq(&spidev->spi_lock);

		if (dofree)
			kfree(spidev);
	}
	mutex_unlock(&device_list_lock);

	return status;
}

static const struct file_operations spidev_fops = {
	.owner =	THIS_MODULE,
	/* REVISIT switch to aio primitives, so that userspace
	 * gets more complete API coverage.  It'll simplify things
	 * too, except for the locking.
	 */
	.write =	spidev_write,
	.read =		spidev_read,
	.unlocked_ioctl = spidev_ioctl,
	.compat_ioctl = spidev_compat_ioctl,
	.open =		spidev_open,
	.release =	spidev_release,
	.llseek =	no_llseek,
};

static const struct file_operations controlFUN_fops = {
	.owner =	THIS_MODULE,
	/* REVISIT switch to aio primitives, so that userspace
	 * gets more complete API coverage.  It'll simplify things
	 * too, except for the locking.
	 */
	.write =	spidev_write,
	.read =		spidev_read,
	.unlocked_ioctl = spidev_ioctl,
	.compat_ioctl = spidev_compat_ioctl,
	.open =		spidev_open,
	.release =	spidev_release,
	.llseek =	no_llseek,
};

/*-------------------------------------------------------------------------*/

/* The main reason to have this class is to make mdev/udev create the
 * /dev/spidevB.C character device nodes exposing our userspace API.
 * It also simplifies memory management.
 */

static struct class *spidev_class;
static char * serialno = "S211";
int len_serialno =0;
/*-------------------------------------------------------------------------*/

static int spidev_probe(struct spi_device *spi)
{
//	struct spidev_data	*spidev;
	int			status;
	unsigned long		minor;

	/* Allocate driver data */
	spidev[number_device] = kzalloc(sizeof(*spidev[number_device]), GFP_KERNEL);
	if (!spidev[number_device])
		return -ENOMEM;

	/* Initialize the driver data */
	spidev[number_device]->spi = spi;
	spin_lock_init(&spidev[number_device]->spi_lock);
	mutex_init(&spidev[number_device]->buf_lock);

	INIT_LIST_HEAD(&spidev[number_device]->device_entry);

	/* If we can allocate a minor number, hook up this device.
	 * Reusing minors is fine so long as udev or mdev is working.
	 */
	mutex_lock(&device_list_lock);
	minor = find_first_zero_bit(minors, N_SPI_MINORS);

	if (minor < N_SPI_MINORS) {
		struct device *dev;

		spidev[number_device]->devt = MKDEV(SPIDEV_MAJOR, minor);
		dev = device_create(spidev_class, &spi->dev, spidev[number_device]->devt,
				    spidev[number_device], "spi_bkav_dev%d.%d",spi->master->bus_num,
				     spi->chip_select);
		status = PTR_ERR_OR_ZERO(dev);
	} else {
		dev_dbg(&spi->dev, "no minor number available!\n");
		status = -ENODEV;
	}
	if (status == 0) {
		set_bit(minor, minors);
		list_add(&spidev[number_device]->device_entry, &device_list);
	}
	mutex_unlock(&device_list_lock);

	if (status == 0)
		spi_set_drvdata(spi, spidev[number_device]);
	else
		kfree(spidev[number_device]);
	if(number_device == 0){
			//len_serialno = property_get("ro.serialno", serialno, "S211");
			spi_config(number_device);
			F_LensInit(len_serialno);
		}else{
			spi_config(number_device);
	}
	number_device++;

	return 0;
}

static int spidev_remove(struct spi_device *spi)
{
	spidev[number_device] = spi_get_drvdata(spi);
//	struct mutex		buf_lock;
	/* make sure ops on existing fds can abort cleanly */
	spin_lock_irq(&spidev[number_device]->spi_lock);
	spidev[number_device]->spi = NULL;
	spi_set_drvdata(spi, NULL);
	spin_unlock_irq(&spidev[number_device]->spi_lock);

	/* prevent new opens */
	mutex_lock(&device_list_lock);
	list_del(&spidev[number_device]->device_entry);
	device_destroy(spidev_class, spidev[number_device]->devt);
	clear_bit(MINOR(spidev[number_device]->devt), minors);
	if (spidev[number_device]->users == 0)
		kfree(spidev[number_device]);
	mutex_unlock(&device_list_lock);
	number_device++;
	return 0;
}

static const struct of_device_id spidev_dt_ids[] = {
	{ .compatible = "rohm,dh2228fv" },
	{ .compatible = "lineartechnology,ltc2488" },
	{ .compatible = "ge,achc" },
	{ .compatible = "semtech,sx1301" },
	{ .compatible = "lwn,bk4" },
	{ .compatible = "dh,dhcom-board" },
	{ .compatible = "menlo,m53cpld" },
	{},
};

MODULE_DEVICE_TABLE(of, spidev_dt_ids);

static struct spi_driver spidev_spi_driver = {
	.driver = {
		.name =		"spidev",
		.owner =	THIS_MODULE,
		.of_match_table = of_match_ptr(spidev_dt_ids),
	},
	.probe =	spidev_probe,
	.remove =	spidev_remove,

	/* NOTE:  suspend/resume methods are not necessary here.
	 * We don't do anything except pass the requests to/from
	 * the underlying controller.  The refrigerator handles
	 * most issues; the controller driver handles the rest.
	 */
};

/*-------------------------------------------------------------------------*/

static int __init spidev_init(void)
{
	int status;

	/* Claim our 256 reserved device numbers.  Then register a class
	 * that will key udev/mdev to add/remove /dev nodes.  Last, register
	 * the driver which manages those device numbers.
	 */
	BUILD_BUG_ON(N_SPI_MINORS > 256);
	status = register_chrdev(SPIDEV_MAJOR, "spi", &spidev_fops);
	if (status < 0)
		return status;

	spidev_class = class_create(THIS_MODULE, "spidev");
	if (IS_ERR(spidev_class)) {
		unregister_chrdev(SPIDEV_MAJOR, spidev_spi_driver.driver.name);
		return PTR_ERR(spidev_class);
	}

	status = spi_register_driver(&spidev_spi_driver);
	if (status < 0) {
		class_destroy(spidev_class);
		unregister_chrdev(SPIDEV_MAJOR, spidev_spi_driver.driver.name);
	}
	return status;
}
module_init(spidev_init);

static void __exit spidev_exit(void)
{
	spi_unregister_driver(&spidev_spi_driver);
	class_destroy(spidev_class);
	unregister_chrdev(SPIDEV_MAJOR, spidev_spi_driver.driver.name);
}
module_exit(spidev_exit);

MODULE_AUTHOR("Andrea Paterniani, <a.paterniani@swapp-eng.it>");
MODULE_DESCRIPTION("User mode SPI device interface");
MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:spidev");
