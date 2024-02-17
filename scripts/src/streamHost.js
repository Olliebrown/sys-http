import { createServer } from 'net'

export default startStreamHost = (listenPort, dataCallback) => {
  const server = createServer((socket) => {
    socket.on('data', dataCallback)  

    socket.on('connect', () => {
      console.log('== Connected ==')
    })
  
    socket.on('end', () => {
      console.log('== Client disconnected ==')
    })
  
    socket.on('error', (err) => {
      console.error('== Socket Error: ==')
      console.error(err)
      console.error('===================')
    })
  
    socket.on('timeout', () => {
      console.log('== Socket timeout ==')
    })
  
    socket.on('close', () => {
      console.log('== Socket closed ==')
    })
  })
  
  server.listen(listenPort, () => {
    console.log(`Streaming host is listening on port ${listenPort}`)
  })

  return server;
}
