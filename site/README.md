# loom site

Static catalog generated from `loom.json` and each unit README. No backend,
no client framework, no install scripts.

```sh
node site/build.mjs
# output: site/dist/
```

Deploy `site/dist` to any static host.

## GitHub Pages (primary)

`.github/workflows/pages.yml` publishes `site/dist` to GitHub Pages on every
push that touches the site or the catalog. No secret is needed: the workflow
uses the repository token.

The site is served at <https://welkon.github.io/loom-package/>. All links are
relative, so the project path works without a custom domain.

To serve it at `loom.pcbsmt.me`:

1. At the DNS provider, replace the Worker record with
   `CNAME loom → welkon.github.io`, proxy **off** (DNS only); Pages cannot
   issue a certificate through a proxy.
2. In Settings → Pages, set the custom domain to `loom.pcbsmt.me` and enable
   Enforce HTTPS.
3. Keep `site/CNAME` (or a `CNAME` file in `dist`) in sync; the Pages action
   publishes whatever the build writes.

Pages limits: 1 GB site, 100 GB per month, 10 builds per hour. `site/dist` is
about 2 MB.

## Cloudflare Workers (fallback)

The checked-in `wrangler.toml` still deploys the same files as a Worker with
the custom domain `loom.pcbsmt.me`:

```sh
export CLOUDFLARE_API_TOKEN=<token>
make deploy-site
```

`site/deploy.sh` builds, runs the site test, and deploys through Wrangler.
GitHub Actions runs it from `site.yml` when a `CLOUDFLARE_API_TOKEN` repository
secret exists, and skips itself otherwise. Keep this path only while the
domain still points at the Worker.

The pages include:

- catalog search, kind filter, tags, applications, compatibility;
- one page per unit with the full README and an API section extracted from the header;
- a Docs index grouped by operator and driver;
- pinned source ref and commit;
- `index.json` for a future registry client.

The source link uses `loom.lock.source`. A private Git host needs the reader
to authenticate; the generated HTML itself contains no credentials.
