import udp from 'node:dgram'

// Address and port of the switch
const SERVER_PORT = 3000
const SERVER_ADDRESS = '192.168.50.91'

// Creating a udp socket
const server = udp.createSocket('udp4');

// Emits when any error occurs
server.on('error', (error) => {
  console.error(error)
  server.close()
})

// emits on new datagram msg
server.on('message', (msg, info) => {
  console.log(msg.toString())
})

//emits when socket is ready and listening for datagram msgs
server.on('listening', () => {
  const address = server.address()
  console.log(`Host is listening at ${address.address}:${address.port}`)

  // Sending connection message
  const client = udp.createSocket('udp4');
  client.send(address.port.toString(), SERVER_PORT, SERVER_ADDRESS, (error) => {
    if (error) {
      console.error(error)
    } else {
      console.log('Port sent')
    }
    client.close()
  })
})

// Emits after the socket is closed using socket.close();
server.on('close', () => {
  console.log('Socket is closed !')
})

// Bind the socket to a random local port
server.bind();
