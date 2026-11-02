# loom site

Static catalog generated from `loom.json` and each unit README. No backend,
no client framework, no install scripts.

```sh
node site/build.mjs
# output: site/dist/
```

Deploy `site/dist` to any static host. The checked-in `wrangler.toml`
deploys it as a Worker with static assets and the custom domain
`loom.pcbsmt.me`:

```sh
export CLOUDFLARE_API_TOKEN=<token>
make deploy-site
```

`site/deploy.sh` builds, runs the site test, and deploys through Wrangler.
GitHub Actions uses the same path with a `CLOUDFLARE_API_TOKEN` repository
secret; the deploy job skips itself when that secret is unset.

The pages include:

- catalog search, kind filter, tags, applications, compatibility;
- one page per unit with the full README and an API section extracted from the header;
- a Docs index grouped by operator and driver;
- pinned source ref and commit;
- `index.json` for a future registry client.

The source link uses `loom.lock.source`. A private Git host needs the reader
to authenticate; the generated HTML itself contains no credentials.
