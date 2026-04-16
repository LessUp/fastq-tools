/* =============================================================================
   FastQTools 分析脚本 - 隐私友好的使用统计
   Privacy-Friendly Analytics for FastQTools
   ============================================================================= */

(function() {
  'use strict';

  // 配置
  const config = {
    enabled: false,  // 默认禁用，需要时启用
    endpoint: '',    // 分析端点
    sampleRate: 0.1  // 采样率
  };

  // 不跟踪（DNT）检测
  if (navigator.doNotTrack === '1' || 
      window.doNotTrack === '1' || 
      navigator.msDoNotTrack === '1') {
    return;
  }

  // 本地存储分析数据
  const Analytics = {
    data: {
      pageViews: [],
      events: [],
      timing: []
    },

    init() {
      this.trackPageView();
      this.trackEngagement();
      this.trackPerformance();
    },

    // 页面浏览追踪
    trackPageView() {
      const data = {
        type: 'pageview',
        url: location.pathname,
        referrer: document.referrer,
        title: document.title,
        timestamp: Date.now(),
        language: navigator.language,
        screenSize: `${screen.width}x${screen.height}`,
        viewportSize: `${window.innerWidth}x${window.innerHeight}`
      };

      this.data.pageViews.push(data);
      
      // 开发环境下输出到控制台
      if (location.hostname === 'localhost' || location.hostname === '127.0.0.1') {
        console.log('[Analytics] Page view:', data);
      }
    },

    // 用户参与度追踪
    trackEngagement() {
      let maxScrollDepth = 0;
      let timeOnPage = 0;
      const startTime = Date.now();

      // 滚动深度
      const trackScroll = () => {
        const scrollPercent = Math.round(
          (window.scrollY / (document.documentElement.scrollHeight - window.innerHeight)) * 100
        );
        
        if (scrollPercent > maxScrollDepth) {
          maxScrollDepth = scrollPercent;
        }
      };

      // 页面离开前记录
      window.addEventListener('beforeunload', () => {
        timeOnPage = Date.now() - startTime;
        
        const engagementData = {
          type: 'engagement',
          timeOnPage: timeOnPage,
          maxScrollDepth: maxScrollDepth,
          timestamp: Date.now()
        };

        this.data.events.push(engagementData);

        // 使用 sendBeacon 确保数据发送
        if (navigator.sendBeacon && config.enabled) {
          navigator.sendBeacon(config.endpoint, JSON.stringify(engagementData));
        }
      });

      window.addEventListener('scroll', this.throttle(trackScroll, 500));
    },

    // 性能追踪
    trackPerformance() {
      if (!('performance' in window)) return;

      window.addEventListener('load', () => {
        setTimeout(() => {
          const perfData = performance.timing;
          const pageLoadTime = perfData.loadEventEnd - perfData.navigationStart;
          const domReadyTime = perfData.domContentLoadedEventEnd - perfData.navigationStart;

          const timingData = {
            type: 'performance',
            pageLoadTime: pageLoadTime,
            domReadyTime: domReadyTime,
            firstPaint: this.getFirstPaint(),
            timestamp: Date.now()
          };

          this.data.timing.push(timingData);

          if (location.hostname === 'localhost') {
            console.log('[Analytics] Performance:', timingData);
          }
        }, 0);
      });
    },

    // 获取首次绘制时间
    getFirstPaint() {
      const perfEntries = performance.getEntriesByType('paint');
      const firstPaint = perfEntries.find(entry => entry.name === 'first-contentful-paint');
      return firstPaint ? Math.round(firstPaint.startTime) : null;
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
    }
  };

  // 初始化
  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', () => Analytics.init());
  } else {
    Analytics.init();
  }

})();
