#include "nbiot.h"

Nbiot::Nbiot() {
  this->serial = new UARTSerial(PB9, PB10, 9600);
  this->parser = new ATCmdParser(this->serial);

  printf("Starting modem setup \n");
  
  this->parser->set_delimiter("\r\n");
	this->parser->set_timeout(5000);

  this->parser->debug_on(1);

  /* Reboot device */
  this->parser->send("AT+NRB");
  wait_ms(5000);
  this->parser->recv("OK");

  /* Enable autoconnect */
  this->parser->send("AT+NCONFIG=\"AUTOCONNECT\",\"TRUE\"");
	this->parser->recv("OK");

  /* Enable scrambling */
	this->parser->send("AT+NCONFIG=\"CR_0354_0338_SCRAMBLING\",\"TRUE\"");
	this->parser->recv("OK");

  /* Reboot device */
  this->parser->send("AT+NRB");
  wait_ms(5000);
  this->parser->recv("OK");

  /* Configure APN */
  this->parser->send("AT+CGDCONT=1,\"IP\",\"mda.ee\"");
	this->parser->recv("OK");
}

/* Transmit data */
bool Nbiot::TransmitData(char *data, int len) {
	unsigned int 		  ufd;
  unsigned int      ulength;
  bool              rc;
  int               tries;

	char 				      hex[1024] = {0};
  char              payload[1024] = {0};

  tries = 0;

	for (unsigned int i = 0, j = 0; i < len; ++i, j += 2){
		sprintf(hex + j, "%02x", data[i] & 0xff);
	}

  snprintf(payload, 1024, "AT+NSOST=%d,\"%s\",%d,%d,\"%s\"", this->fd, "172.16.15.14", 1234, len, hex);

  while(true) {
	  this->parser->send(payload);
	  rc = this->parser->recv("%d,%d", &ufd, &ulength);
    if (rc || tries >= 5) {
      break;
    }
    tries++;
    wait_ms(2000);
  }
	
  printf("Data is transmitted, socket: %d, length: %d \n", ufd, ulength);
}

/* Connect to nbiot network */
bool Nbiot::Connect() {
  printf("Starting to connect \n");
  
  /* Enable modem */
  parser->send("AT+CFUN=1");
	parser->recv("OK");

  wait_ms(1000);

  /* Search for Telenor */
  parser->send("AT+COPS=1,2,\"24201\"");
	parser->recv("OK");

  wait_ms(1000);

  /* Print APN and IP address */
  parser->send("AT+CGDCONT?");
	parser->recv("OK");

  wait_ms(1000);

  /* Attach to context */
  parser->send("AT+CGACT=1,1");
	parser->recv("OK");

  wait_ms(1000);

  while(true) {
    parser->send("AT+CEREG?");
	  if(parser->recv("+CEREG: 0,1")) {
      break;
    }
  }

  /* Open a socket */
	parser->send("AT+NSOCR=\"DGRAM\",17,8888,1");
	parser->recv("%d", &this->fd);

  wait_ms(1000);

  return true;
}