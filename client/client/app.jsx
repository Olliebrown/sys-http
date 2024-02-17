import React from 'react'
import { createRoot } from 'react-dom/client'

import Root from './Components/Root.jsx'

import '@fontsource/roboto/300.css'
import '@fontsource/roboto/400.css'
import '@fontsource/roboto/500.css'
import '@fontsource/roboto/700.css'

const container = document.getElementById('appRoot')
const root = createRoot(container)
root.render(<Root />)
