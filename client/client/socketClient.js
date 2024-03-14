import { io } from 'socket.io-client'

// Connect to the server
export function connectToSocketServer () {
  const socket = io()
  return socket
}
