import React, { useEffect } from 'react'
import { useRouter } from 'next/router'
import { Globe, ArrowRight } from 'lucide-react'

interface LanguageOption {
  code: string
  name: string
  localName: string
  flag: string
  description: string
}

const languages: LanguageOption[] = [
  {
    code: 'en',
    name: 'English',
    localName: 'English',
    flag: '🇺🇸',
    description: 'Official documentation in English'
  },
  {
    code: 'zh',
    name: 'Chinese',
    localName: '简体中文',
    flag: '🇨🇳',
    description: '中文官方文档'
  }
]

export function LanguageSelector() {
  const router = useRouter()

  useEffect(() => {
    // Auto-redirect based on browser language on first visit
    const visited = sessionStorage.getItem('fqt-language-selected')
    if (!visited) {
      const browserLang = navigator.language.toLowerCase()
      if (browserLang.startsWith('zh')) {
        sessionStorage.setItem('fqt-language-selected', 'true')
        router.push('/zh')
      }
    }
  }, [router])

  const handleSelect = (code: string) => {
    sessionStorage.setItem('fqt-language-selected', 'true')
    router.push(`/${code}`)
  }

  return (
    <div className="min-h-[80vh] flex flex-col items-center justify-center px-4">
      <div className="text-center mb-12">
        <div className="inline-flex items-center justify-center w-16 h-16 rounded-2xl bg-gradient-to-br from-primary-500 to-bio-600 text-white mb-6 shadow-lg shadow-primary-500/25">
          <Globe className="w-8 h-8" />
        </div>
        <h1 className="text-4xl md:text-5xl font-bold mb-4">
          <span className="gradient-text-bio">FastQTools</span>
        </h1>
        <p className="text-xl text-gray-600 dark:text-gray-400 max-w-md mx-auto">
          Choose your language / 选择语言
        </p>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 gap-6 max-w-2xl w-full">
        {languages.map((lang) => (
          <button
            key={lang.code}
            onClick={() => handleSelect(lang.code)}
            className="language-card text-left group"
          >
            <span className="lang-flag">{lang.flag}</span>
            <h3 className="text-xl font-bold mb-1">{lang.localName}</h3>
            <p className="text-gray-500 dark:text-gray-400 text-sm mb-4">{lang.name}</p>
            <p className="text-gray-600 dark:text-gray-300 text-sm">{lang.description}</p>
            <div className="mt-4 flex items-center text-primary-600 dark:text-primary-400 font-medium text-sm opacity-0 group-hover:opacity-100 transition-opacity">
              Continue
              <ArrowRight className="w-4 h-4 ml-1 group-hover:translate-x-1 transition-transform" />
            </div>
          </button>
        ))}
      </div>

      <div className="mt-12 text-center text-sm text-gray-500 dark:text-gray-400">
        <p>High-performance FASTQ processing toolkit for sequencing QC</p>
        <div className="flex items-center justify-center gap-4 mt-4">
          <a
            href="https://github.com/LessUp/fastq-tools"
            target="_blank"
            rel="noreferrer"
            className="hover:text-primary-600 transition-colors"
          >
            GitHub
          </a>
          <span>•</span>
          <a
            href="https://github.com/LessUp/fastq-tools/releases"
            target="_blank"
            rel="noreferrer"
            className="hover:text-primary-600 transition-colors"
          >
            Releases
          </a>
          <span>•</span>
          <a
            href="https://github.com/LessUp/fastq-tools/issues"
            target="_blank"
            rel="noreferrer"
            className="hover:text-primary-600 transition-colors"
          >
            Issues
          </a>
        </div>
      </div>
    </div>
  )
}