/* =============================================================================
   FastQTools 自定义 JavaScript - 终极交互体验
   Ultimate JavaScript for FastQTools Documentation
   ============================================================================= */

(function() {
  'use strict';

  // ============================================================================
  // 配置 / Configuration
  // ============================================================================
  const config = {
    animations: {
      scrollReveal: true,
      hoverEffects: true,
      typingEffect: false
    },
    search: {
      highlightDuration: 2000,
      autoFocus: false
    },
    ui: {
      smoothScroll: true,
      progressBar: true,
      readingTime: true
    }
  };

  // ============================================================================
  // 工具函数 / Utility Functions
  // ============================================================================
  const utils = {
    // 防抖函数
    debounce(func, wait) {
      let timeout;
      return function executedFunction(...args) {
        const later = () => {
          clearTimeout(timeout);
          func(...args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
      };
    },

    // 节流函数
    throttle(func, limit) {
      let inThrottle;
      return function(...args) {
        if (!inThrottle) {
          func.apply(this, args);
          inThrottle = true;
          setTimeout(() => inThrottle = false, limit);
        }
      };
    },

    // 平滑滚动到元素
    scrollToElement(element, offset = 80) {
      const elementPosition = element.getBoundingClientRect().top;
      const offsetPosition = elementPosition + window.pageYOffset - offset;
      
      window.scrollTo({
        top: offsetPosition,
        behavior: 'smooth'
      });
    },

    // 检测元素是否在视口中
    isInViewport(element, threshold = 0.1) {
      const rect = element.getBoundingClientRect();
      return (
        rect.top <= (window.innerHeight || document.documentElement.clientHeight) * (1 - threshold) &&
        rect.bottom >= (window.innerHeight || document.documentElement.clientHeight) * threshold
      );
    },

    // 添加动画类
    addAnimation(element, animationClass, duration = 600) {
      element.classList.add('fq-animate', animationClass);
      setTimeout(() => {
        element.classList.remove('fq-animate', animationClass);
      }, duration);
    },

    // 获取阅读时间
    calculateReadingTime(text) {
      const wordsPerMinute = 200;
      const words = text.trim().split(/\s+/).length;
      const minutes = Math.ceil(words / wordsPerMinute);
      return minutes;
    }
  };

  // ============================================================================
  // 滚动显示动画 / Scroll Reveal Animation
  // ============================================================================
  class ScrollReveal {
    constructor() {
      this.elements = document.querySelectorAll('.md-typeset h2, .md-typeset h3, .md-typeset .admonition, .md-typeset table, .md-typeset pre');
      this.observer = null;
      this.init();
    }

    init() {
      if (!('IntersectionObserver' in window)) {
        // 回退方案
        this.elements.forEach(el => el.classList.add('fq-visible'));
        return;
      }

      this.observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
          if (entry.isIntersecting) {
            entry.target.classList.add('fq-scroll-animate', 'fq-visible');
            this.observer.unobserve(entry.target);
          }
        });
      }, {
        threshold: 0.1,
        rootMargin: '0px 0px -50px 0px'
      });

      this.elements.forEach(el => {
        el.classList.add('fq-scroll-animate');
        this.observer.observe(el);
      });
    }
  }

  // ============================================================================
  // 代码块增强 / Code Block Enhancement
  // ============================================================================
  class CodeBlockEnhancer {
    constructor() {
      this.codeBlocks = document.querySelectorAll('.md-typeset pre');
      this.init();
    }

    init() {
      this.codeBlocks.forEach(block => {
        this.addLanguageLabel(block);
        this.enhanceCopyButton(block);
        this.addLineNumbers(block);
      });
    }

    // 添加语言标签
    addLanguageLabel(block) {
      const code = block.querySelector('code');
      if (!code) return;

      const lang = this.detectLanguage(code);
      if (lang) {
        block.setAttribute('data-lang', lang);
      }
    }

    // 检测编程语言
    detectLanguage(codeElement) {
      const className = codeElement.className;
      const match = className.match(/language-(\w+)/);
      if (match) return match[1];

      // 基于内容检测
      const text = codeElement.textContent;
      if (text.includes('#include') || text.includes('std::')) return 'cpp';
      if (text.includes('import ') && text.includes('def ')) return 'python';
      if (text.includes('function') || text.includes('const ') || text.includes('let ')) return 'javascript';
      if (text.includes('cmake_minimum_required')) return 'cmake';
      
      return 'code';
    }

    // 增强复制按钮
    enhanceCopyButton(block) {
      const button = block.querySelector('.md-clipboard');
      if (!button) return;

      button.addEventListener('click', () => {
        // 添加成功动画
        button.classList.add('md-clipboard--active');
        utils.addAnimation(button, 'fq-bounce', 400);
        
        setTimeout(() => {
          button.classList.remove('md-clipboard--active');
        }, 2000);
      });
    }

    // 添加行号
    addLineNumbers(block) {
      const code = block.querySelector('code');
      if (!code || block.querySelector('.linenos')) return;

      const lines = code.innerHTML.split('\n');
      if (lines.length < 5) return; // 短代码不显示行号

      // 已经有行号的不处理
      if (code.parentElement.classList.contains('highlighttable')) return;

      // 可以在这里添加行号逻辑
    }
  }

  // ============================================================================
  // 导航增强 / Navigation Enhancement
  // ============================================================================
  class NavigationEnhancer {
    constructor() {
      this.toc = document.querySelector('.md-sidebar--secondary .md-nav');
      this.headings = document.querySelectorAll('.md-typeset h2, .md-typeset h3');
      this.init();
    }

    init() {
      if (!this.toc || this.headings.length === 0) return;
      
      this.highlightCurrentSection();
      this.setupScrollSpy();
    }

    // 高亮当前章节
    highlightCurrentSection() {
      const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
          if (entry.isIntersecting) {
            const id = entry.target.id;
            this.toc.querySelectorAll('.md-nav__link').forEach(link => {
              link.classList.remove('md-nav__link--active');
              if (link.getAttribute('href') === `#${id}`) {
                link.classList.add('md-nav__link--active');
              }
            });
          }
        });
      }, {
        rootMargin: '-20% 0px -80% 0px'
      });

      this.headings.forEach(heading => observer.observe(heading));
    }

    // 滚动监听
    setupScrollSpy() {
      let currentActive = null;

      window.addEventListener('scroll', utils.throttle(() => {
        const scrollPos = window.scrollY + 100;

        this.headings.forEach(heading => {
          const top = heading.offsetTop;
          const bottom = top + heading.offsetHeight;

          if (scrollPos >= top && scrollPos < bottom) {
            if (currentActive !== heading.id) {
              currentActive = heading.id;
              this.updateTOC(heading.id);
            }
          }
        });
      }, 100));
    }

    updateTOC(activeId) {
      this.toc.querySelectorAll('.md-nav__link').forEach(link => {
        link.classList.remove('md-nav__link--active');
        if (link.getAttribute('href') === `#${activeId}`) {
          link.classList.add('md-nav__link--active');
          // 展开父级菜单
          let parent = link.closest('.md-nav__item--nested');
          while (parent) {
            parent.classList.add('md-nav__item--active');
            parent = parent.parentElement.closest('.md-nav__item--nested');
          }
        }
      });
    }
  }

  // ============================================================================
  // 搜索增强 / Search Enhancement
  // ============================================================================
  class SearchEnhancer {
    constructor() {
      this.searchInput = document.querySelector('.md-search__input');
      this.init();
    }

    init() {
      if (!this.searchInput) return;

      // 自动聚焦（可选）
      if (config.search.autoFocus) {
        document.addEventListener('keydown', (e) => {
          if (e.key === '/' && !e.ctrlKey && !e.metaKey) {
            e.preventDefault();
            this.searchInput.focus();
          }
        });
      }

      // 搜索快捷键提示
      this.addKeyboardHint();
    }

    addKeyboardHint() {
      const form = document.querySelector('.md-search__form');
      if (!form) return;

      const hint = document.createElement('kbd');
      hint.className = 'md-search__shortcut';
      hint.textContent = '/';
      hint.style.cssText = `
        position: absolute;
        right: 12px;
        top: 50%;
        transform: translateY(-50%);
        background: rgba(255,255,255,0.1);
        padding: 2px 6px;
        border-radius: 4px;
        font-size: 12px;
        color: #8892b0;
        pointer-events: none;
      `;
      
      form.style.position = 'relative';
      form.appendChild(hint);

      this.searchInput.addEventListener('focus', () => {
        hint.style.opacity = '0';
      });

      this.searchInput.addEventListener('blur', () => {
        if (!this.searchInput.value) {
          hint.style.opacity = '1';
        }
      });
    }
  }

  // ============================================================================
  // 键盘快捷键 / Keyboard Shortcuts
  // ============================================================================
  class KeyboardShortcuts {
    constructor() {
      this.init();
    }

    init() {
      document.addEventListener('keydown', (e) => {
        // Ctrl/Cmd + K: 聚焦搜索
        if ((e.ctrlKey || e.metaKey) && e.key === 'k') {
          e.preventDefault();
          const searchInput = document.querySelector('.md-search__input');
          if (searchInput) {
            searchInput.focus();
          }
        }

        // Escape: 关闭搜索
        if (e.key === 'Escape') {
          const searchOverlay = document.querySelector('.md-search__overlay');
          if (searchOverlay && searchOverlay.checked) {
            searchOverlay.checked = false;
          }
        }

        // g + h: 回到首页
        if (e.key === 'h' && !e.ctrlKey && !e.metaKey) {
          const lastKey = this.lastKey;
          this.lastKey = 'h';
          if (lastKey === 'g') {
            e.preventDefault();
            window.location.href = '/';
          }
          setTimeout(() => this.lastKey = null, 500);
        }
      });
    }
  }

  // ============================================================================
  // 阅读进度条 / Reading Progress Bar
  // ============================================================================
  class ReadingProgressBar {
    constructor() {
      this.progressBar = null;
      this.init();
    }

    init() {
      if (!config.ui.progressBar) return;

      // 创建进度条元素
      this.progressBar = document.createElement('div');
      this.progressBar.className = 'fq-reading-progress';
      this.progressBar.style.cssText = `
        position: fixed;
        top: 0;
        left: 0;
        width: 0%;
        height: 3px;
        background: linear-gradient(90deg, #00bfa5, #00e5ff);
        z-index: 10000;
        transition: width 0.1s ease;
        box-shadow: 0 0 10px rgba(0, 191, 165, 0.5);
      `;

      document.body.appendChild(this.progressBar);

      // 监听滚动
      window.addEventListener('scroll', utils.throttle(() => {
        this.updateProgress();
      }, 50));
    }

    updateProgress() {
      const scrollTop = window.pageYOffset || document.documentElement.scrollTop;
      const scrollHeight = document.documentElement.scrollHeight - document.documentElement.clientHeight;
      const progress = (scrollTop / scrollHeight) * 100;
      
      this.progressBar.style.width = progress + '%';
    }
  }

  // ============================================================================
  // 外部链接处理 / External Links Handler
  // ============================================================================
  class ExternalLinksHandler {
    constructor() {
      this.init();
    }

    init() {
      document.querySelectorAll('.md-typeset a[href^="http"]').forEach(link => {
        // 添加外部链接图标
        if (!link.querySelector('.external-link-icon')) {
          const icon = document.createElement('span');
          icon.className = 'external-link-icon';
          icon.innerHTML = ' ↗';
          icon.style.cssText = 'font-size: 0.8em; opacity: 0.6;';
          link.appendChild(icon);
        }

        // 新窗口打开
        link.setAttribute('target', '_blank');
        link.setAttribute('rel', 'noopener noreferrer');
      });
    }
  }

  // ============================================================================
  // 图片懒加载 / Lazy Loading Images
  // ============================================================================
  class LazyImageLoader {
    constructor() {
      this.images = document.querySelectorAll('.md-typeset img:not([loading])');
      this.init();
    }

    init() {
      if (!('IntersectionObserver' in window)) {
        this.images.forEach(img => img.setAttribute('loading', 'lazy'));
        return;
      }

      const imageObserver = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
          if (entry.isIntersecting) {
            const img = entry.target;
            img.classList.add('fq-fadeIn');
            imageObserver.unobserve(img);
          }
        });
      });

      this.images.forEach(img => {
        img.setAttribute('loading', 'lazy');
        img.style.opacity = '0';
        img.style.transition = 'opacity 0.3s ease';
        
        img.addEventListener('load', () => {
          img.style.opacity = '1';
        });
        
        imageObserver.observe(img);
      });
    }
  }

  // ============================================================================
  // 平滑锚点滚动 / Smooth Anchor Scrolling
  // ============================================================================
  class SmoothAnchorScroll {
    constructor() {
      this.init();
    }

    init() {
      document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', (e) => {
          const targetId = anchor.getAttribute('href').slice(1);
          const targetElement = document.getElementById(targetId);
          
          if (targetElement) {
            e.preventDefault();
            utils.scrollToElement(targetElement);
            
            // 更新 URL
            history.pushState(null, null, `#${targetId}`);
          }
        });
      });
    }
  }

  // ============================================================================
  // 初始化 / Initialization
  // ============================================================================
  function init() {
    // DOM 加载完成后初始化
    if (document.readyState === 'loading') {
      document.addEventListener('DOMContentLoaded', initComponents);
    } else {
      initComponents();
    }
  }

  function initComponents() {
    // 初始化所有组件
    new ScrollReveal();
    new CodeBlockEnhancer();
    new NavigationEnhancer();
    new SearchEnhancer();
    new KeyboardShortcuts();
    new ReadingProgressBar();
    new ExternalLinksHandler();
    new LazyImageLoader();
    new SmoothAnchorScroll();

    // 控制台欢迎信息
    console.log('%c FastQTools ', 'background: linear-gradient(135deg, #00bfa5, #00e5ff); color: #fff; padding: 8px 16px; border-radius: 4px; font-size: 16px; font-weight: bold;');
    console.log('%c Documentation loaded successfully! 🚀 ', 'color: #00bfa5; font-size: 12px;');
  }

  // 启动
  init();

})();
