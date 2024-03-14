import fs from 'fs'
import esbuild from 'esbuild'
import dotenv from 'dotenv'

// Look for server and dev modes
let _DEV_ = process.argv.includes('dev')
const _SERVE_ = process.argv.includes('serve')

if (_SERVE_) {
  console.log('Doing development build with static server')
  _DEV_ = true
} else if (_DEV_) {
  console.log('Doing development build')
} else {
  console.log('Doing production build')
}

async function doBuild () {
  // Read in secrets from the .env file
  const data = await fs.promises.readFile('.env', 'utf8')
  const buf = Buffer.from(data)
  const rawConfig = dotenv.parse(buf)

  // Wrap all env variables in quotes
  const config = []
  for (const key in rawConfig) {
    config[key] = `"${rawConfig[key]}"`
  }

  // Set up the build options
  const options = {
    entryPoints: ['client/app.jsx'],
    outdir: 'public',
    bundle: true,
    define: {
      'process.env.NODE_ENV': _DEV_ ? '"development"' : '"production"',
      ...config
    },
    target: 'es2018',
    external: ['env'],
    loader: { '.woff': 'binary', '.woff2': 'binary' },
    minify: !_DEV_,
    sourcemap: _DEV_
  }

  if (_DEV_) {
    // Create esbuild context and start watch
    const ctx = await esbuild.context(options)

    process.on('SIGINT', async () => {
      await ctx.dispose()
      process.exit(0)
    })

    // Serve or watch files
    if (_SERVE_) {
      await ctx.serve({
        servedir: 'public',
        port: rawConfig['DEV_SERVER_PORT'] ? parseInt(rawConfig['DEV_SERVER_PORT']) : 3000
      })
    } else {
      await ctx.watch()
    }
  } else {
    // Do the build
    esbuild.build(options).then(() => {
      console.log('Build succeeded.')
    }).catch((e) => {
      console.error('Error building:')
      console.error(e)
      process.exit(1)
    })
  }
}

doBuild()
