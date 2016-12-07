/*
Copyright (c) Fraunhofer ITWM - Carsten Lojewski <lojewski@itwm.fhg.de>, 2013-2016

This file is part of GPI-2.

GPI-2 is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License
version 3 as published by the Free Software Foundation.

GPI-2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GPI-2. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _GPI2_DEV_H_
#define _GPI2_DEV_H_

#include "GASPI.h"

/* Device interface */

/////////////////// WORK SANDBOX /////////////////////////
///TODO: think about these

 char *
pgaspi_dev_get_rrcd(int);

 char *
pgaspi_dev_get_lrcd(int);

 size_t
pgaspi_dev_get_sizeof_rc(void);

/* Groups */
int
pgaspi_dev_poll_groups(void);

int
pgaspi_dev_post_group_write(void *, int, int, void *, unsigned char);

//////////////////////////////////////////////////////////

#ifdef GPI2_CUDA
gaspi_return_t
pgaspi_dev_segment_alloc (const gaspi_segment_id_t,
			  const gaspi_size_t,
			  const gaspi_alloc_t);

gaspi_return_t
pgaspi_dev_segment_delete (const gaspi_segment_id_t);
#endif /* GPI2_CUDA */

int
pgaspi_dev_init_core(gaspi_config_t *);

int
pgaspi_dev_cleanup_core(gaspi_config_t *);

int
pgaspi_dev_register_mem(gaspi_rc_mseg_t *);

int
pgaspi_dev_unregister_mem(const gaspi_rc_mseg_t *);

int
pgaspi_dev_connect_context(const int);

int
pgaspi_dev_disconnect_context(const int);

int
pgaspi_dev_create_endpoint(const int);

int
pgaspi_dev_comm_queue_delete(const unsigned int id);

int
pgaspi_dev_comm_queue_create(const unsigned int, const unsigned short);

int
pgaspi_dev_comm_queue_connect(const unsigned short q, const int i);

/* Device interface (GASPI routines) */
int
pgaspi_dev_queue_size(const gaspi_queue_id_t);

gaspi_return_t
pgaspi_dev_purge (const gaspi_queue_id_t queue,
		  const gaspi_timeout_t timeout_ms);

gaspi_return_t
pgaspi_dev_write (const gaspi_segment_id_t, const gaspi_offset_t, const gaspi_rank_t,
		  const gaspi_segment_id_t, const gaspi_offset_t, const gaspi_size_t,
		  const gaspi_queue_id_t);

gaspi_return_t
pgaspi_dev_read (const gaspi_segment_id_t, const gaspi_offset_t, const gaspi_rank_t,
		 const gaspi_segment_id_t, const gaspi_offset_t, const gaspi_size_t,
		 const gaspi_queue_id_t);


gaspi_return_t
pgaspi_dev_wait (const gaspi_queue_id_t, const gaspi_timeout_t);


gaspi_return_t
pgaspi_dev_write_list (const gaspi_number_t,
		       gaspi_segment_id_t * const,
		       gaspi_offset_t * const,
		       const gaspi_rank_t,
		       gaspi_segment_id_t * const,
		       gaspi_offset_t * const,
		       gaspi_size_t* const,
		       const gaspi_queue_id_t);

gaspi_return_t
pgaspi_dev_read_list (const gaspi_number_t,
		      gaspi_segment_id_t * const,
		      gaspi_offset_t * const,
		      const gaspi_rank_t,
		      gaspi_segment_id_t * const,
		      gaspi_offset_t * const,
		      gaspi_size_t* const,
		      const gaspi_queue_id_t);

gaspi_return_t
pgaspi_dev_notify (const gaspi_segment_id_t,
		   const gaspi_rank_t,
		   const gaspi_notification_id_t,
		   const gaspi_notification_t,
		   const gaspi_queue_id_t);

gaspi_return_t
pgaspi_dev_write_notify (const gaspi_segment_id_t,
			 const gaspi_offset_t,
			 const gaspi_rank_t,
			 const gaspi_segment_id_t,
			 const gaspi_offset_t,
			 const gaspi_size_t,
			 const gaspi_notification_id_t,
			 const gaspi_notification_t,
			 const gaspi_queue_id_t);


gaspi_return_t
pgaspi_dev_write_list_notify (const gaspi_number_t,
			      gaspi_segment_id_t * const,
			      gaspi_offset_t * const,
			      const gaspi_rank_t,
			      gaspi_segment_id_t * const,
			      gaspi_offset_t * const,
			      gaspi_size_t* const,
			      const gaspi_segment_id_t,
			      const gaspi_notification_id_t,
			      const gaspi_notification_t,
			      const gaspi_queue_id_t);

gaspi_return_t
pgaspi_dev_read_notify (const gaspi_segment_id_t,
			const gaspi_offset_t,
			const gaspi_rank_t,
			const gaspi_segment_id_t,
			const gaspi_offset_t,
			const gaspi_size_t,
			const gaspi_notification_id_t,
			const gaspi_queue_id_t);

gaspi_return_t
pgaspi_dev_read_list_notify (const gaspi_number_t num,
			     gaspi_segment_id_t * const segment_id_local,
			     gaspi_offset_t * const offset_local,
			     const gaspi_rank_t rank,
			     gaspi_segment_id_t * const segment_id_remote,
			     gaspi_offset_t * const offset_remote,
			     gaspi_size_t * const size,
			     const gaspi_segment_id_t segment_id_notification,
			     const gaspi_notification_id_t notification_id,
			     const gaspi_queue_id_t queue);
gaspi_return_t
pgaspi_dev_atomic_fetch_add (const gaspi_segment_id_t,
			     const gaspi_offset_t,
			     const gaspi_rank_t,
			     const gaspi_atomic_value_t);


gaspi_return_t
pgaspi_dev_atomic_compare_swap (const gaspi_segment_id_t,
				const gaspi_offset_t,
				const gaspi_rank_t,
				const gaspi_atomic_value_t,
				const gaspi_atomic_value_t);

gaspi_return_t
pgaspi_dev_passive_send (const gaspi_segment_id_t,
			 const gaspi_offset_t,
			 const gaspi_rank_t,
			 const gaspi_size_t,
			 const gaspi_timeout_t);

gaspi_return_t
pgaspi_dev_passive_receive (const gaspi_segment_id_t segment_id_local,
			    const gaspi_offset_t offset_local,
			    gaspi_rank_t * const rem_rank,
			    const gaspi_size_t size,
			    const gaspi_timeout_t timeout_ms);

#ifdef GPI2_CUDA
gaspi_return_t
pgaspi_dev_gpu_write(const gaspi_segment_id_t segment_id_local,
		     const gaspi_offset_t offset_local,
		     const gaspi_rank_t rank,
		     const gaspi_segment_id_t segment_id_remote,
		     const gaspi_offset_t offset_remote,
		     const gaspi_size_t size,
		     const gaspi_queue_id_t queue,
		     const gaspi_timeout_t timeout_ms);

gaspi_return_t
pgaspi_dev_gpu_write_notify(const gaspi_segment_id_t segment_id_local,
			    const gaspi_offset_t offset_local,
			    const gaspi_rank_t rank,
			    const gaspi_segment_id_t segment_id_remote,
			    const gaspi_offset_t offset_remote,
			    const gaspi_size_t size,
			    const gaspi_notification_id_t notification_id,
			    const gaspi_notification_t notification_value,
			    const gaspi_queue_id_t queue,
			    const gaspi_timeout_t timeout_ms);
int
_gaspi_find_dev_numa_node(void);

#endif //GPI2_CUDA

#endif //_GPI2_DEV_H_
