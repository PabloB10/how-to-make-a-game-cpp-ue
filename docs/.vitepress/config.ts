import { defineConfig } from 'vitepress'

export default defineConfig({
  title: 'UE Platformer in C++',
  description: 'Modern, concise guidance for building a UE 5.6+ platformer in C++',
  base: '/how-to-make-a-game-cpp-ue/',
  cleanUrls: true,
  lastUpdated: true,
  head: [
    ['meta', { name: 'theme-color', content: '#0F1B2D' }],
    [
      'link',
      { rel: 'icon', href: '/how-to-make-a-game-cpp-ue/favicon.svg', type: 'image/svg+xml' },
    ],
    ['link', { rel: 'icon', href: '/how-to-make-a-game-cpp-ue/logo.svg', type: 'image/svg+xml' }],
    [
      'meta',
      {
        name: 'description',
        content:
          'Learn to build a UE 5.6+ platformer in C++ with modern patterns, comprehensive guides, and professional code quality.',
      },
    ],
    ['meta', { property: 'og:image', content: '/how-to-make-a-game-cpp-ue/logo-large.svg' }],
    ['meta', { name: 'twitter:card', content: 'summary_large_image' }],
  ],
  themeConfig: {
    search: {
      provider: 'local',
    },
    nav: [
      {
        text: 'Guide',
        items: [
          { text: 'Overview', link: '/guide/' },
          { text: 'Prerequisites', link: '/guide/prerequisites' },
          { text: 'Getting Started', link: '/guide/getting-started' },
          { text: 'UE Setup', link: '/guide/ue-setup' },
          { text: 'Companion Repo Setup', link: '/guide/companion-repo-setup' },
        ],
      },
      {
        text: 'Milestones',
        items: [
          { text: 'Overview', link: '/guide/milestones' },
          { text: 'Milestone 1 - Character & Input', link: '/guide/milestone-1-detailed' },
          { text: 'Milestone 2 - Interactions', link: '/guide/milestone-2-detailed' },
          { text: 'Milestone 3 - Stats & AI', link: '/guide/milestone-3-detailed' },
          { text: 'Milestone 4 - NPCs & Events', link: '/guide/milestone-4-detailed' },
          { text: 'Milestone 5 - Testing & Polish', link: '/guide/milestone-5-detailed' },
        ],
      },
      {
        text: 'Templates',
        items: [
          { text: 'Template Overview', link: '/templates/' },
          { text: 'Milestone 1 Starter', link: '/templates/milestone-1-starter/' },
          { text: 'Milestone 2 Starter', link: '/templates/milestone-2-starter/' },
          { text: 'Milestone 3 Starter', link: '/templates/milestone-3-starter/' },
          { text: 'Milestone 4 Starter', link: '/templates/milestone-4-starter/' },
          { text: 'Milestone 5 Starter', link: '/templates/milestone-5-starter/' },
          { text: 'Complete Project', link: '/templates/complete-project/' },
        ],
      },
      {
        text: 'Reference',
        items: [
          { text: 'Architecture', link: '/reference/architecture' },
          { text: 'Gameplay Objects', link: '/reference/gameplay-objects' },
          { text: 'Input & Camera', link: '/reference/input-and-camera' },
          { text: 'Code Examples', link: '/reference/code-examples' },
          { text: 'C++ + Unreal Primer', link: '/reference/cpp-primer-for-unreal' },
        ],
      },
      {
        text: 'Support',
        items: [
          { text: 'Testing', link: '/guide/testing' },
          { text: 'Error Handling', link: '/guide/error-handling-best-practices' },
          { text: 'Console Variables', link: '/guide/console-variables-guide' },
          { text: 'Debugging', link: '/guide/debugging' },
          { text: 'Common Mistakes', link: '/guide/common-mistakes' },
          { text: 'Troubleshooting', link: '/guide/troubleshooting' },
          { text: 'FAQ', link: '/appendix/faq' },
          { text: 'Glossary', link: '/appendix/glossary' },
        ],
      },
    ],
    sidebar: {
      '/guide/': [
        {
          text: 'Getting Started',
          items: [
            { text: 'Overview', link: '/guide/' },
            { text: 'Prerequisites', link: '/guide/prerequisites' },
            { text: 'Getting Started', link: '/guide/getting-started' },
            { text: 'UE Setup', link: '/guide/ue-setup' },
            { text: 'Companion Repo Setup', link: '/guide/companion-repo-setup' },
          ],
        },
        {
          text: 'Milestones',
          items: [
            { text: 'Overview', link: '/guide/milestones' },
            { text: 'Milestone 1 - Character & Input', link: '/guide/milestone-1-detailed' },
            { text: 'Milestone 2 - Interactions', link: '/guide/milestone-2-detailed' },
            { text: 'Milestone 3 - Stats & AI', link: '/guide/milestone-3-detailed' },
            { text: 'Milestone 4 - NPCs & Events', link: '/guide/milestone-4-detailed' },
            { text: 'Milestone 5 - Testing & Polish', link: '/guide/milestone-5-detailed' },
          ],
        },
        {
          text: 'Support',
          items: [
            { text: 'Testing', link: '/guide/testing' },
            { text: 'Error Handling', link: '/guide/error-handling-best-practices' },
            { text: 'Console Variables', link: '/guide/console-variables-guide' },
            { text: 'Debugging', link: '/guide/debugging' },
            { text: 'Common Mistakes', link: '/guide/common-mistakes' },
            { text: 'Troubleshooting', link: '/guide/troubleshooting' },
          ],
        },
      ],
      '/templates/': [
        {
          text: 'Templates',
          items: [
            { text: 'Overview', link: '/templates/' },
            { text: 'How to Use Templates', link: '/templates/#how-to-choose' },
          ],
        },
        {
          text: 'Milestone Starters',
          items: [
            { text: 'Milestone 1 - Character & Input', link: '/templates/milestone-1-starter/' },
            { text: 'Milestone 2 - Interactions', link: '/templates/milestone-2-starter/' },
            { text: 'Milestone 3 - Stats & AI', link: '/templates/milestone-3-starter/' },
            { text: 'Milestone 4 - NPCs & Events', link: '/templates/milestone-4-starter/' },
            { text: 'Milestone 5 - Testing & Polish', link: '/templates/milestone-5-starter/' },
          ],
        },
        {
          text: 'Complete Reference',
          items: [{ text: 'Complete Project', link: '/templates/complete-project/' }],
        },
      ],
      '/reference/': [
        {
          text: 'Reference',
          items: [
            { text: 'Architecture', link: '/reference/architecture' },
            { text: 'C++ + Unreal Primer', link: '/reference/cpp-primer-for-unreal' },
            { text: 'Gameplay Objects', link: '/reference/gameplay-objects' },
            { text: 'Input & Camera', link: '/reference/input-and-camera' },
            { text: 'Code Examples', link: '/reference/code-examples' },
          ],
        },
      ],
      '/appendix/': [
        {
          text: 'Appendix',
          items: [
            { text: 'FAQ', link: '/appendix/faq' },
            { text: 'Glossary', link: '/appendix/glossary' },
          ],
        },
      ],
    },
    socialLinks: [
      // { icon: 'github', link: 'https://github.com/<owner>/<repo>' }
    ],
    editLink: {
      pattern: 'https://github.com/<owner>/<repo>/edit/main/docs/:path',
      text: 'Edit this page on GitHub',
    },
    footer: {
      message: '',
    },
  },
})
