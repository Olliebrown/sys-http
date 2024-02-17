import http from 'http'
import socketIO from 'socket.io'

import Dotenv from 'dotenv'
import debug from 'debug'
import { socketInit } from './socketEvents'

// Load environment variables
Dotenv.config()
const WS_LISTEN_PORT = process.env.WS_LISTEN_PORT || 3000

// Debug loggers
const log = debug('repeater')

// Create a standard HTTP server with socketIO enabled
const server = http.createServer()
const io = socketIO(server)

// Socket callback
io.on('connection', socket => {
  socketInit(socket)
})

// Start the server
server.listen(WS_LISTEN_PORT, () => {
  log(`Socket.io server listening on localhost:${WS_LISTEN_PORT}`)
})

// Respond to SIGINT (Ctrl+C) to close the server
process.on('SIGINT', () => {
  log('Received SIGINT: Shutting down...')
  server.close()
  process.exit()
})
