import Request from './Request.js'

export default class PeekRequest extends Request {
  constructor (nickname, offsets, interval = -1) {
    super(nickname, offsets, interval, Request.Type.PEEK)
  }
}
