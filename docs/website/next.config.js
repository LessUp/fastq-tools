const withNextra = require('nextra').default({
  theme: 'nextra-theme-docs',
  themeConfig: './theme.config.tsx',
  defaultShowCopyCode: true,
  staticImage: true,
})

/** @type {import('next').NextConfig} */
const nextConfig = {
  output: 'export',
  distDir: '../.site',
  basePath: '/fastq-tools',
  assetPrefix: '/fastq-tools',
  images: {
    unoptimized: true,
  },
  // Note: i18n is not compatible with output: 'export'
  // We use file-based routing (pages/en/, pages/zh/) instead
}

module.exports = withNextra(nextConfig)
