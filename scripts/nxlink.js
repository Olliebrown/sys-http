import { createServer } from 'net'

const PORT = 42424
const server = createServer((socket) => {
  socket.on('connect', () => {
    console.log('Connected.')
  })

  socket.on('data', (data) => {
    // Echo back the received data
    console.log(data.toString())
  })

  socket.on('end', () => {
    console.log('Client disconnected')
  })

  socket.on('error', (err) => {
    console.error('Socket Error:')
    console.error(err)
  })

  socket.on('timeout', () => {
    console.log('Socket timeout.')
  })

  socket.on('close', () => {
    console.log('Socket closed')
  })
})

server.listen(PORT, () => {
  console.log(`NXLink server is listening on port ${PORT}`)
})
