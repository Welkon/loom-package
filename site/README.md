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
npx wrangler deploy
```

The pages include:

- catalog search, kind filter, tags, applications, compatibility;
- one page per unit with the full README;
- pinned source ref and commit;
- `index.json` for a future registry client.

The source link uses `loom.lock.source`. A private Git host needs the reader
to authenticate; the generated HTML itself contains no credentials.
