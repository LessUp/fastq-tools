import { useRouter } from 'next/router'

const config = {
  logo: (
    <>
      <span className="font-bold text-xl flex items-center gap-2">
        <svg
          width="24"
          height="24"
          viewBox="0 0 24 24"
          fill="none"
          xmlns="http://www.w3.org/2000/svg"
          className="text-primary-600 dark:text-primary-400"
        >
          <path
            d="M12 2C13.1 2 14 2.9 14 4V6C14 7.1 13.1 8 12 8C10.9 8 10 7.1 10 6V4C10 2.9 10.9 2 12 2ZM6 10H4C2.9 10 2 10.9 2 12C2 13.1 2.9 14 4 14H6C7.1 14 8 13.1 8 12C8 10.9 7.1 10 6 10ZM20 10H18C16.9 10 16 10.9 16 12C16 13.1 16.9 14 18 14H20C21.1 14 22 13.1 22 12C22 10.9 21.1 10 20 10ZM12 16C10.9 16 10 16.9 10 18V20C10 21.1 10.9 22 12 22C13.1 22 14 21.1 14 20V18C14 16.9 13.1 16 12 16ZM7.05 6.64L5.64 5.23C4.86 4.45 4.86 3.18 5.64 2.4C6.42 1.62 7.69 1.62 8.47 2.4L9.88 3.81C10.66 4.59 10.66 5.86 9.88 6.64C9.1 7.42 7.83 7.42 7.05 6.64ZM16.95 17.36L18.36 18.77C19.14 19.55 19.14 20.82 18.36 21.6C17.58 22.38 16.31 22.38 15.53 21.6L14.12 20.19C13.34 19.41 13.34 18.14 14.12 17.36C14.9 16.58 16.17 16.58 16.95 17.36ZM5.64 18.77L7.05 17.36C7.83 16.58 9.1 16.58 9.88 17.36C10.66 18.14 10.66 19.41 9.88 20.19L8.47 21.6C7.69 22.38 6.42 22.38 5.64 21.6C4.86 20.82 4.86 19.55 5.64 18.77ZM18.36 5.23L16.95 6.64C16.17 7.42 14.9 7.42 14.12 6.64C13.34 5.86 13.34 4.59 14.12 3.81L15.53 2.4C16.31 1.62 17.58 1.62 18.36 2.4C19.14 3.18 19.14 4.45 18.36 5.23Z"
            fill="currentColor"
          />
        </svg>
        FastQTools
      </span>
    </>
  ),
  project: {
    link: 'https://github.com/LessUp/fastq-tools',
  },
  docsRepositoryBase: 'https://github.com/LessUp/fastq-tools/tree/master/docs/website',
  useNextSeoProps() {
    const { asPath } = useRouter()
    const title = asPath === '/'
      ? 'FastQTools - High-performance FASTQ Processing Toolkit'
      : '%s – FastQTools'
    return {
      titleTemplate: title,
      description: 'High-performance FASTQ processing toolkit for sequencing QC with 1.7M reads/sec throughput',
      openGraph: {
        type: 'website',
        locale: 'en_US',
        url: 'https://lessup.github.io/fastq-tools',
        siteName: 'FastQTools',
      },
      twitter: {
        cardType: 'summary_large_image',
      },
    }
  },
  head: (
    <>
      <meta name="viewport" content="width=device-width, initial-scale=1.0" />
      <meta name="description" content="High-performance FASTQ processing toolkit for sequencing QC with zero-copy design" />
      <meta property="og:title" content="FastQTools" />
      <meta property="og:description" content="Process FASTQ files at 1.7M reads/sec with a CLI for everyday QC and a zero-copy C++ API" />
      <link rel="icon" type="image/svg+xml" href="/fastq-tools/favicon.svg" />
      <link rel="preconnect" href="https://fonts.googleapis.com" />
      <link rel="preconnect" href="https://fonts.gstatic.com" crossOrigin="anonymous" />
      <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet" />
    </>
  ),
  search: {
    placeholder: 'Search documentation...',
  },
  toc: {
    title: 'On This Page',
  },
  editLink: {
    text: 'Edit this page on GitHub →',
  },
  feedback: {
    content: 'Question? Give us feedback →',
    labels: 'documentation',
  },
  footer: {
    text: (
      <div className="flex flex-col gap-2">
        <span>
          MIT {new Date().getFullYear()} ©{' '}
          <a href="https://github.com/LessUp" target="_blank" rel="noreferrer" className="hover:text-primary-600 transition-colors">
            LessUp
          </a>
        </span>
        <span className="text-xs text-gray-500 dark:text-gray-400">
          Built with Nextra, Next.js & Tailwind CSS
        </span>
      </div>
    ),
  },
  darkMode: true,
  nextThemes: {
    defaultTheme: 'system',
    storageKey: 'fqt-theme',
  },
  sidebar: {
    defaultMenuCollapseLevel: 1,
    toggleButton: true,
  },
  navigation: {
    prev: true,
    next: true,
  },
}

export default config
