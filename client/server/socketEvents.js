// eslint-disable-next-line no-unused-vars
import * as socketIO from 'socket.io'

import debug from 'debug'

// Bring in the request classes
import Request from './Requests.js'

// Debug loggers
const log = debug('memwitch:socket')

// Map of active requests
const activeRequests = {}

/**
 * Set up the socket server and initialize the connection event.
 *
 * @param {socketIO.Server} io
 */
export function setupSocketServer (io) {
  log('Starting socket server')
  io.on('connection', (socket) => socketInit(socket))
  io.engine.on('connection_error', (err) => {
    log('Connection Error:', err.code)
    log(err.message)
  })
}

/**
 * Initialize a new socket connection
 * @param {socketIO.Socket} socket The newly connected socket
 */
function socketInit (socket) {
  // Log the new connection
  log('Connect:', socket.id)

  // Initialize socket events
  socket.on('disconnect', () => disconnect(socket))
  socket.on('error', (err) => error(socket, err))

  // Custom events for info and memory peeks/pokes
  socket.on('info', (data) => info(socket, data))
  socket.on('peek', (data) => peek(socket, data))
  socket.on('poke', (data) => poke(socket, data))
  socket.on('stop', (data) => stop(socket, data))

  // Initialize the active requests for this socket (or clear old ones)
  activeRequests[socket.id] = []
}

// Individual socket events
/**
 * Handle a socket disconnection
 * @param {socketIO.Socket} socket The socket that is disconnecting
 */
function disconnect (socket) {
  log('Disconnect:', socket.id)

  // Stop any active requests
  if (Array.isArray(activeRequests[socket.id])) {
    activeRequests[socket.id].forEach((request) => {
      request.stop()
    })
  }

  // Remove the socket from the active requests
  delete activeRequests[socket.id]
}

/**
 * Handle any socket errors
 * @param {socketIO.Socket} socket The socket that created the error
 * @param {Error} err The error that was emitted
 */
function error (socket, err) {
  log('Error:', socket.id, err)
}

/**
 * Request information about the active game and memory peeks and pokes
 * @param {socketIO.Socket} socket The socket that created the event
 * @param {PokeRequest} data Info about what you want to know
 */
function info (socket, data) {
  const request = Request.makeServerRequest(Request.InfoRequest, data)
  log('Info:', request)
  socket.emit('created', { id: request.id })
}

/**
 * Establishes a new memory peek session with the switch that might
 * be a continuous stream.
 * @param {socketIO.Socket} socket The socket that created the event
 * @param {PeekRequest} data Info about where to peek in memory
 */
function peek (socket, data) {
  if (!data.offsets) {
    throw new Error('Invalid peek request')
  }

  const request = Request.makeServerRequest(Request.PeekRequest, data)
  log('Peek:', request)
  socket.emit('created', { id: request.id })
}

/**
 * Poke a value into memory
 * @param {socketIO.Socket} socket The socket that created the event
 * @param {PokeRequest} data Info about where to poke in memory and what to write
 */
function poke (socket, data) {
  if (!data.offsets) {
    throw new Error('Invalid poke request')
  }

  const request = Request.makeServerRequest(Request.PokeRequest, data)
  log('Poke:', request)
  socket.emit('created', { id: request.id })
}

/**
 * Stops a previously established request interval stream.
 * @param {socketIO.Socket} socket The socket that created the event
 * @param {int} requestId The specific stream to stop
 */
function stop (socket, requestId) {
  log('Stop:', socket.id, requestId)
  const socketRequests = activeRequests[socket.id]
  const requestIndex =
    socketRequests?.findIndex((r) => r.id === requestId) ?? -1

  if (requestIndex < 0) {
    log('Request not found:', requestId)
    socket.emit('error', { message: 'Request not found' })
  } else {
    socketRequests[requestIndex].stop()
    socketRequests.splice(requestIndex, 1)
    socket.emit('stopped', { id: requestId })
  }
}
