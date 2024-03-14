import crypto from 'crypto'

// Import all the request types and export them as a single object
import Request from '../common/index.js'

// Convenience factory method
Request.makeServerRequest = (constructor, data) => {
  // Decorate the request with a unique ID
  return {
    id: crypto.randomUUID(),
    ...Request.makeRequest(constructor, data)
  }
}

// Export the decorated Request
export default Request
