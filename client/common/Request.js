export default class Request {
  constructor (nickname, offsets, interval = -1, type = Request.Type.UNKNOWN) {
    this.nickname = nickname?.toString()
    this.type = type

    this.offsets = offsets
    if (!Array.isArray(offsets)) {
      this.offsets = [offsets]
    }

    this.interval = Number(interval)
    if (!this.interval) {
      this.interval = -1
    }
  }
}

Request.Type = Object.freeze({
  INFO: Symbol('info'),
  PEEK: Symbol('peek'),
  POKE: Symbol('poke'),
  UNKNOWN: Symbol('unknown')
})
