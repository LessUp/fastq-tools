import DefaultTheme from 'vitepress/theme'
import { inBrowser } from 'vitepress'
import type { EnhanceAppContext } from 'vitepress'
import './style.css'

const LANG_KEY = 'vitepress:lang-pref'

/** 大小写不敏感的浏览器语言检测 */
function detectBrowserLang(): 'zh' | 'en' {
  const navLang = inBrowser
    ? (navigator.language || (navigator as any).userLanguage || '').toLowerCase()
    : ''
  return navLang.startsWith('zh') ? 'zh' : 'en'
}

/** 读取 localStorage 中的语言偏好 */
function getLangPreference(): 'zh' | 'en' | null {
  if (!inBrowser) return null
  try {
    const stored = localStorage.getItem(LANG_KEY)
    return (stored === 'zh' || stored === 'en') ? stored : null
  } catch {
    return null // 无痕模式等
  }
}

/** 保存语言偏好到 localStorage */
function setLangPreference(lang: 'zh' | 'en'): void {
  if (!inBrowser) return
  try {
    localStorage.setItem(LANG_KEY, lang)
  } catch {
    // localStorage 不可用时静默失败
  }
}

export default {
  extends: DefaultTheme,

  enhanceApp({ router, siteData }: EnhanceAppContext) {
    if (!inBrowser) return

    const base = siteData.value.base || '/'

    // ---- 首次访问根路径时自动跳转 ----
    const checkAndRedirect = () => {
      const path = window.location.pathname
      const basePath = base === '/' ? '/' : base.replace(/\/$/, '')

      // 判断是否在根路径（多种变体）
      const isRoot = path === '/' ||
        path === '/index.html' ||
        path === basePath ||
        path === basePath + '/' ||
        path === basePath + '/index.html'

      if (!isRoot) return

      // 优先使用存储的偏好，其次检测浏览器语言
      const lang = getLangPreference() || detectBrowserLang()
      const target = base.replace(/\/$/, '') + '/' + lang + '/'

      router.go(target)
    }

    // 立即执行（在 hydration 之前，减少闪烁）
    checkAndRedirect()

    // ---- 监听路由变化，保存语言偏好 ----
    router.onAfterRouteChanged = (to: string) => {
      if (to.includes('/zh/')) setLangPreference('zh')
      else if (to.includes('/en/')) setLangPreference('en')
    }
  },
}
