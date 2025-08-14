import DefaultTheme from 'vitepress/theme'
import { h } from 'vue'
import './custom.css'

export default {
  extends: DefaultTheme,
  Layout() {
    return h(DefaultTheme.Layout, null, {
      'layout-bottom': () =>
        h('div', { class: 'footer-avatar' }, [
          h('a', { href: 'https://github.com/grzott', target: '_blank', rel: 'noopener' }, [
            h('img', {
              src: 'https://github.com/grzott.png',
              alt: '@grzott',
              width: 56,
              height: 56,
              class: 'footer-avatar__img',
            }),
          ]),
          h('div', { class: 'footer-avatar__text' }, [
            'Maintained by ',
            h(
              'a',
              { href: 'https://github.com/grzott', target: '_blank', rel: 'noopener' },
              '@grzott'
            ),
          ]),
        ]),
    })
  },
}
