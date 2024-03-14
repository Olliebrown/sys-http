import Request from './Request.js'

export default class InfoRequest extends Request {
  constructor (nickname, offsets, interval = -1) {
    super(nickname, offsets, interval, Request.Type.INFO)
  }
}
