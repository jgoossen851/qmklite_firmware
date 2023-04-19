// @ts-check
// Note: type annotations allow type checking and IDEs autocompletion

const lightCodeTheme = require('prism-react-renderer/themes/github');
const darkCodeTheme = require('prism-react-renderer/themes/dracula');

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: 'QMK Firmware',
  tagline: 'Potatoes are cool',
  favicon: 'img/favicon.ico',

  // Set the production url of your site here
  url: 'https://docs.qmk.fm',
  // Set the /<baseUrl>/ pathname under which your site is served
  // For GitHub pages deployment, it is often '/<projectName>/'
  baseUrl: '/',

  // GitHub pages deployment config.
  // If you aren't using GitHub pages, you don't need these.
  organizationName: 'qmk', // Usually your GitHub org/user name.
  projectName: 'qmk_firmware', // Usually your repo name.

  onBrokenLinks: 'throw',
  onBrokenMarkdownLinks: 'warn',

  // Even if you don't use internalization, you can use this field to set useful
  // metadata like html lang. For example, if your site is Chinese, you may want
  // to replace "en" with "zh-Hans".
  i18n: {
    defaultLocale: 'en',
    locales: ['en'],//, 'ja', 'zh-cn'],
  },

  presets: [
    [
      'classic',
      /** @type {import('@docusaurus/preset-classic').Options} */
      ({
        docs: {
          sidebarPath: require.resolve('./sidebars.js'),
          path: ".",
          routeBasePath: "/",
          exclude: [
            "node_modules",
            "src",
            "static",
            "ja",
            "zh-cn",
            "gitbook",
            // "ChangeLog"
          ],
          editUrl:
            'https://github.com/qmk/qmk_firmware/edit/master/docs/',
        },
        blog: false,
        theme: {
          customCss: require.resolve('./src/css/custom.css'),
        },
      }),
    ],
  ],

  themeConfig:
    /** @type {import('@docusaurus/preset-classic').ThemeConfig} */
    ({
      image: 'img/qmk-social-card.jpg',
      navbar: {
        title: 'QMK Firmware Docs',
        logo: {
          alt: 'QMK Firmware',
          src: 'img/qmk-new-light.svg',
          srcDark: 'img/qmk-new-dark.svg',
          width: 32,
          height: 32,
        },
        items: [
          {
            type: 'localeDropdown',
            position: 'right',
          },
          {
            href: 'https://github.com/qmk/qmk_firmware',
            label: 'GitHub',
            position: 'right',
          },
        ],
      },
      docs: {
        sidebar: {
          autoCollapseCategories: false
        }
      },
      footer: {
        style: 'dark',
        links: [
          {
            title: 'Docs',
            items: [
              {
                label: 'Docs',
                to: '/',
              },
            ],
          },
          {
            title: 'Community',
            items: [
              {
                label: 'Discord',
                href: 'https://discord.gg/Uq7gcHh',
              },
              {
                label: 'Reddit',
                href: 'https://reddit.com/r/olkb',
              },
            ],
          },
          {
            title: 'More',
            items: [
              {
                label: 'GitHub',
                href: 'https://github.com/qmk/qmk_firmware',
              },
            ],
          },
        ],
        copyright: `Copyright © ${new Date().getFullYear()} QMK. Built with Docusaurus.`,
      },
      prism: {
        theme: lightCodeTheme,
        darkTheme: darkCodeTheme,
      },  
      // applied for - will fill out once we hear back
      //
      // algolia: {
      //   // The application ID provided by Algolia
      //   appId: 'YOUR_APP_ID',

      //   // Public API key: it is safe to commit it
      //   apiKey: 'YOUR_SEARCH_API_KEY',

      //   indexName: 'YOUR_INDEX_NAME',

      //   // Optional: see doc section below
      //   contextualSearch: true,

      //   // Optional: Specify domains where the navigation should occur through window.location instead on history.push. Useful when our Algolia config crawls multiple documentation sites and we want to navigate with window.location.href to them.
      //   externalUrlRegex: 'external\\.com|domain\\.com',

      //   // Optional: Replace parts of the item URLs from Algolia. Useful when using the same search index for multiple deployments using a different baseUrl. You can use regexp or string in the `from` param. For example: localhost:3000 vs myCompany.com/docs
      //   replaceSearchResultPathname: {
      //     from: '/docs/', // or as RegExp: /\/docs\//
      //     to: '/',
      //   },

      //   // Optional: Algolia search parameters
      //   searchParameters: {},

      //   // Optional: path for search page that enabled by default (`false` to disable it)
      //   searchPagePath: 'search',

      //   //... other Algolia params
      // },
    }),
};

module.exports = config;
