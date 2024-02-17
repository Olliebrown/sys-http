import debug from 'debug'

// Debug loggers
const log = debug('memwitch:socket')

/**
 * Initialize a new socket connection
 * @param {Socket} socket The newly connected socket
 */
export function socketInit (socket) {
  log('Connect:', socket.id)

  // Initialize socket events
  socket.on('disconnect', () => disconnect(socket))
  socket.on('error', (err) => error(socket, err))
  socket.on('peek', (data) => peek(socket, data))
  socket.on('stopPeek', (data) => stopPeek(socket, data))
  socket.on('poke', (data) => poke(socket, data))
}

// Individual socket events
/**
 * Handle a socket disconnection
 * @param {Socket} socket The socket that is disconnecting
 */
function disconnect (socket) {
  log('Disconnect:', socket.id)
}

/**
 * Handle any socket errors
 * @param {Socket} socket The socket that created the error
 * @param {Error} err The error that was emitted
 */
function error (socket, err) {
  log('Error:', socket.id, err)
}

/**
 * Establishes a new memory peek session with the switch that might
 * be a continuous stream.
 * @param {Socket} socket The socket that created the event
 * @param {PeekRequest} data Info about where to peek in memory
 */
function peek (socket, data) {
  log('Peek:', socket.id, data)
}

/**
 * Stops a previously established memory peek stream.
 * @param {Socket} socket The socket that created the event
 * @param {int} streamId The specific stream to stop
 */
function stopPeek (socket, streamId) {
  log('Stop Peek:', socket.id, streamId)
}

/**
 * Poke a value into memory
 * @param {Socket} socket The socket that created the event
 * @param {PokeRequest} data Info about where to poke in memory and what to write
 */
function poke (socket, data) {
  log('Poke:', socket.id, data)
}
