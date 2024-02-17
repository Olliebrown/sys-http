import React from 'react'

import { CssBaseline, Container, Typography } from '@mui/material'

export default function Root (props) {
  return (
    <React.Fragment>
      <CssBaseline />
      <Container>
        <Typography variant='h1'>Mem-Witch Client</Typography>
        <Typography variant='body'>Welcome to the Mem-Witch Client</Typography>
      </Container>
    </React.Fragment>
  )
}
