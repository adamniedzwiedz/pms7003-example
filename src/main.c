#include "common/str_util.h"
#include "mgos.h"
#include "mgos_uart.h"
#include "pms7003.h"

#define UART_NO 0
#define PMS7003_FRAME_LEN 32

static void timer_cb(void *arg) {
  LOG(LL_INFO, ("Requesting for PMS7003 data...\r\n"));
  pms7003_request_read(UART_NO);
  (void) arg;
}

static void uart_dispatcher(int uart_no, void *arg) {
  char *data_str;
  static struct mbuf data = {0};

  assert(uart_no == UART_NO);

  // read UART data if available
  size_t available = mgos_uart_read_avail(uart_no);
  if (available == 0) return;

  mgos_uart_read_mbuf(uart_no, &data, available);

  // find PMS7003 frame 
  unsigned char *frame = pms7003_find_frame((unsigned char*)data.buf, data.len);

  if (frame == NULL) return;

  data_str = malloc(PMS7003_FRAME_LEN * 2 + 1);
  cs_to_hex(data_str, frame, PMS7003_FRAME_LEN); 

  LOG(LL_DEBUG, ("PMS7003 data: %s\r\n", data_str));

  if (!pms7003_validate_checksum(frame)) {
    LOG(LL_DEBUG, ("PMS7003: Invalid frame. Checksum mismatch\r\n"));
  }
  else {
    // print PM measured results
    LOG(LL_INFO, ("-------------------------------------------\r\n"));
    LOG(LL_INFO, ("PMS7003 PM1_0: %ld\r\n", pms7003_get_pm1_0(frame)));
    LOG(LL_INFO, ("PMS7003 PM2_5: %ld\r\n", pms7003_get_pm2_5(frame)));
    LOG(LL_INFO, ("PMS7003 PM10_0: %ld\r\n", pms7003_get_pm10_0(frame)));
  }

  mbuf_free(&data);
  free(data_str);
  (void) arg;
}

static void set_uart_cb(void *arg) {
  // configure UART - PMS7003 uses 9600bps
  struct mgos_uart_config ucfg;
  mgos_uart_config_set_defaults(UART_NO, &ucfg);
  ucfg.baud_rate = 9600;

  if (!mgos_uart_configure(UART_NO, &ucfg)) {
    LOG(LL_ERROR, ("UART not configured\r\n"));
    return;
  } 

  // set passive mode
  pms7003_set_mode(UART_NO, PASSIVE);

  // register UART handler (when data received) and enable receiver
  mgos_uart_set_dispatcher(UART_NO, uart_dispatcher, NULL /* arg */);
  mgos_uart_set_rx_enabled(UART_NO, true);

  (void) arg;
}

enum mgos_app_init_result mgos_app_init(void) {
  // We need to wait on the sensor to run, so the first command must be send with at least 2s delay
  mgos_set_timer(3000 /* ms */, false /* repeat */, set_uart_cb, NULL /* arg */); 
  mgos_set_timer(5000 /* ms */, true /* repeat */, timer_cb, NULL /* arg */); 

  return MGOS_APP_INIT_SUCCESS;
}
