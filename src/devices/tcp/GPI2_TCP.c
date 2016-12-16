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
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "GASPI.h"
#include "GPI2.h"
#include "GPI2_Dev.h"
#include "GPI2_SN.h"
#include "GPI2_TCP.h"
#include "GPI2_Utility.h"
#include "tcp_device.h"

/* TODO: empty functions smell */
inline char *
pgaspi_dev_get_rrcd(int rank)
{
  return NULL;
}


inline char *
pgaspi_dev_get_lrcd(int rank)
{
  return NULL;
}

inline size_t
pgaspi_dev_get_sizeof_rc(void)
{
  return 0;
}

int
pgaspi_dev_create_endpoint(gaspi_context_t const * const gctx, const int i)
{
  return 0;
}

int
pgaspi_dev_disconnect_context(gaspi_context_t const * const gctx, const int i)
{
  return 0;
}

int
pgaspi_dev_connect_context(gaspi_context_t const * const gctx, const int i)
{
  return tcp_dev_connect_to(i, pgaspi_gethostname(i), TCP_DEV_PORT + gctx->poff[i]);
}

int
pgaspi_dev_comm_queue_connect(gaspi_context_t const * const gctx, const unsigned short q, const int i)
{
  return 0;
}

int
pgaspi_dev_comm_queue_delete(gaspi_context_t const * const gctx, const unsigned int id)
{
  tcp_dev_destroy_queue(glb_gaspi_ctx_tcp.qpC[id]);
  glb_gaspi_ctx_tcp.qpC[id] = NULL;

  tcp_dev_destroy_cq(glb_gaspi_ctx_tcp.scqC[id]);
  glb_gaspi_ctx_tcp.scqC[id] = NULL;

  return 0;
}

int
pgaspi_dev_comm_queue_create(gaspi_context_t const * const gctx,
			     const unsigned int id,
			     const unsigned short remote_node)
{
  if( glb_gaspi_ctx_tcp.scqC[id] == NULL)
    {
      glb_gaspi_ctx_tcp.scqC[id] = tcp_dev_create_cq(gctx->config->queue_size_max, NULL);
      if( glb_gaspi_ctx_tcp.scqC[id] == NULL )
	{
	  gaspi_print_error("Failed to create IO completion queue.");
	  return -1;
	}
    }

  if( glb_gaspi_ctx_tcp.qpC[id] == NULL)
    {
      glb_gaspi_ctx_tcp.qpC[id] = tcp_dev_create_queue( glb_gaspi_ctx_tcp.scqC[id], NULL);
      if( glb_gaspi_ctx_tcp.qpC[id] == NULL )
	{
	  gaspi_print_error("Failed to create queue %d for IO.", id);
	  return -1;
	}
    }

  return 0;
}

static void
pgaspi_dev_print_info()
{
  gaspi_context_t const * const gctx = &glb_gaspi_ctx;

  gaspi_printf("<<<<<<<<<<<<<<<< TCP-info >>>>>>>>>>>>>>>>>>>\n");
  gaspi_printf("  Hostname: %s\n", pgaspi_gethostname(gctx->rank));

  char* ip = tcp_dev_get_local_ip(pgaspi_gethostname(gctx->rank));
  if( ip != NULL )
    {
      char* iface = tcp_dev_get_local_if(ip);
      gaspi_printf("  %-8s: %s\n", iface, ip );
      free(iface);
    }
  else
    {
      gaspi_printf("  Failed to retrieve more info\n");
    }
  gaspi_printf("<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

int
pgaspi_dev_init_core(gaspi_context_t const * const gctx)
{
  memset (&glb_gaspi_ctx_tcp, 0, sizeof (gaspi_tcp_ctx));

  struct tcp_dev_args* dev_args = malloc(sizeof(struct tcp_dev_args));
  if( NULL == dev_args )
    {
      gaspi_print_error("Failed to allocate memory.");
      return -1;
    }

  dev_args->peers_num = gctx->tnc;
  dev_args->id = gctx->rank;
  dev_args->port = TCP_DEV_PORT + gctx->localSocket;

  glb_gaspi_ctx_tcp.device_channel = tcp_dev_init_device(dev_args);

  if( glb_gaspi_ctx_tcp.device_channel < 0 )
    {
      gaspi_print_error("Failed to initialize device.");
      return -1;
    }

  /* user did not choose so we set the network type */
  if( !gctx->config->user_net )
    {
      gctx->config->network = GASPI_ETHERNET;
    }

  if( gctx->config->net_info )
    {
      pgaspi_dev_print_info();
    }

  /* Passive channel (SRQ) */
  glb_gaspi_ctx_tcp.srqP = gaspi_sn_connect2port("localhost", TCP_DEV_PORT + gctx->localSocket, CONN_TIMEOUT);
  if( glb_gaspi_ctx_tcp.srqP == -1 )
    {
      gaspi_print_error("Failed to create passive channel connection");
      return -1;
    }

  glb_gaspi_ctx_tcp.channelP = tcp_dev_create_passive_channel();
  if( glb_gaspi_ctx_tcp.channelP == NULL )
    {
      gaspi_print_error("Failed to create passive channel.");
      return -1;
    }

  /* Completion Queues */
  glb_gaspi_ctx_tcp.scqGroups = tcp_dev_create_cq(gctx->config->queue_size_max, NULL);
  if( glb_gaspi_ctx_tcp.scqGroups == NULL )
    {
      gaspi_print_error("Failed to create groups send completion queue.");
      return -1;
    }

  glb_gaspi_ctx_tcp.rcqGroups = tcp_dev_create_cq(gctx->config->queue_size_max, NULL);
  if( glb_gaspi_ctx_tcp.rcqGroups == NULL )
    {
      gaspi_print_error("Failed to create groups receive completion queue.");
      return -1;
    }

  glb_gaspi_ctx_tcp.scqP = tcp_dev_create_cq(gctx->config->queue_size_max, NULL);
  if( glb_gaspi_ctx_tcp.scqP == NULL )
    {
      gaspi_print_error("Failed to create passive send completion queue.");
      return -1;
    }

  glb_gaspi_ctx_tcp.rcqP = tcp_dev_create_cq(gctx->config->queue_size_max, glb_gaspi_ctx_tcp.channelP);
  if( glb_gaspi_ctx_tcp.rcqP == NULL )
    {
      gaspi_print_error("Failed to create passive recv completion queue.");
      return -1;
    }

  unsigned int c;
  for(c = 0; c < gctx->config->queue_num; c++)
    {
      glb_gaspi_ctx_tcp.scqC[c] = tcp_dev_create_cq(gctx->config->queue_size_max, NULL);
      if( glb_gaspi_ctx_tcp.scqC[c] == NULL )
	{
	  gaspi_print_error("Failed to create IO completion queue.");
	  return -1;
	}
    }

  /* Queues (QPs) */
  glb_gaspi_ctx_tcp.qpGroups = tcp_dev_create_queue(glb_gaspi_ctx_tcp.scqGroups,
						    glb_gaspi_ctx_tcp.rcqGroups);
  if( glb_gaspi_ctx_tcp.qpGroups == NULL )
    {
      gaspi_print_error("Failed to create queue for groups.");
      return -1;
    }

  for(c = 0; c < gctx->config->queue_num; c++)
    {
      glb_gaspi_ctx_tcp.qpC[c] = tcp_dev_create_queue( glb_gaspi_ctx_tcp.scqC[c],
						       NULL);
      if( glb_gaspi_ctx_tcp.qpC[c] == NULL )
	{
	  gaspi_print_error("Failed to create queue %d for IO.", c);
	  return -1;
	}
    }

  glb_gaspi_ctx_tcp.qpP = tcp_dev_create_queue( glb_gaspi_ctx_tcp.scqP,
						glb_gaspi_ctx_tcp.rcqP);
  if( glb_gaspi_ctx_tcp.qpP == NULL )
    {
      gaspi_print_error("Failed to create queue for passive.");
      return -1;
    }

  gaspi_tcp_dev_status_t _dev_status = gaspi_tcp_dev_status_get();

  while( GASPI_TCP_DEV_STATUS_DOWN == _dev_status )
    {
      gaspi_delay();
      _dev_status = gaspi_tcp_dev_status_get();
    }

  if( GASPI_TCP_DEV_STATUS_FAILED == _dev_status )
    {
      return -1;
    }

  free(dev_args);

  return 0;
}

int
pgaspi_dev_cleanup_core(gaspi_context_t * const gctx)
{
  if( tcp_dev_stop_device(glb_gaspi_ctx_tcp.device_channel) != 0 )
    {
      gaspi_print_error("Failed to stop device.");
    }

  /* Destroy posting queues and associated channels */
  tcp_dev_destroy_queue(glb_gaspi_ctx_tcp.qpGroups);
  tcp_dev_destroy_queue(glb_gaspi_ctx_tcp.qpP);

  unsigned int c;
  for(c = 0; c < gctx->config->queue_num; c++)
    {
      tcp_dev_destroy_queue(glb_gaspi_ctx_tcp.qpC[c]);
    }

  if( glb_gaspi_ctx_tcp.srqP )
    {
      if( close(glb_gaspi_ctx_tcp.srqP ) < 0 )
	{
	  gaspi_print_error("Failed to close srqP.");
	}
    }

  if( glb_gaspi_ctx_tcp.channelP )
    {
      tcp_dev_destroy_passive_channel(glb_gaspi_ctx_tcp.channelP);
    }

  /* Now we can destroy the resources for completion and incoming data */
  tcp_dev_destroy_cq(glb_gaspi_ctx_tcp.scqGroups);
  tcp_dev_destroy_cq(glb_gaspi_ctx_tcp.rcqGroups);
  tcp_dev_destroy_cq(glb_gaspi_ctx_tcp.scqP);
  tcp_dev_destroy_cq(glb_gaspi_ctx_tcp.rcqP);

  for(c = 0; c < gctx->config->queue_num; c++)
    {
      tcp_dev_destroy_cq(glb_gaspi_ctx_tcp.scqC[c]);
    }
  close(glb_gaspi_ctx_tcp.device_channel);

  return 0;
}
