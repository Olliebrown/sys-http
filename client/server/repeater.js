import Express from 'express'
import http from 'http'
import * as socketIO from 'socket.io'

import Dotenv from 'dotenv'
import debug from 'debug'
import { socketInit } from './socketEvents.js'

// Load environment variables
Dotenv.config()
const LISTEN_PORT = process.env.LISTEN_PORT || 3000

// Debug loggers
const log = debug('memwitch:root')

// Create an express HTTP server with socketIO enabled
const app = new Express()
const server = http.createServer(app)
const io = new socketIO.Server(server)

// Socket connection initialization
io.on('connection', socket => socketInit(socket))

// Express routes
app.use((req, res, next) => {
  log(`Request: ${req.method} ${req.url}`)
  next()
})

// Serve all files in the public folder
app.use(Express.static('public'))

// Start the server
server.listen(LISTEN_PORT, () => {
  log(`Memwitch server running localhost:${LISTEN_PORT}`)
})

// Respond to SIGINT (Ctrl+C) to close the server
process.on('SIGINT', () => {
  log('Received SIGINT: Shutting down...')
  server.close()
  process.exit()
})
