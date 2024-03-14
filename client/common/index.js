// Import all the request types and export them as a single object
import Request from './Request.js'
import PeekRequest from './PeekRequest.js'
import PokeRequest from './PokeRequest.js'
import InfoRequest from './InfoRequest.js'

// Decorate Request with the other request types
Request.PeekRequest = PeekRequest
Request.PokeRequest = PokeRequest
Request.InfoRequest = InfoRequest

// Convenience factory method
Request.makeRequest = (constructor, data) => {
  if (!data || !data.nickname) {
    throw new Error(`Invalid ${constructor.name} request`)
  }

  return new constructor(data.nickname, data.offsets ?? [], data.interval ?? -1)
}

// Export the decorated Request
export default Request
