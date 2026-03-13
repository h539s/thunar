test_bins = [
  'test-resolve-symlink',
]

foreach bin : test_bins
  e = executable(
    bin,
    sources: [
      '@0@.c'.format(bin),
    ],
    c_args: thunar_c_args + '-UG_DISABLE_ASSERT',
    include_directories: [
      include_directories('..'),
    ],
    dependencies: thunar_dependencies + [xfconf],
    link_with: [ libthunar, libthunarx ],
    install: false,
  )

  test(bin, e)
endforeach

test_text_renderer = executable(
  'test-text-renderer',
  sources: [
    'test-text-renderer.c',
    'thunar-snapshot.c',
  ],
  c_args: thunar_c_args + ['-UG_DISABLE_ASSERT'],
  include_directories: [
    include_directories('..'),
  ],
  dependencies: thunar_dependencies + [xfconf],
  link_with: [ libthunar, libthunarx ],
  install: false,
)

test('test-text-renderer', test_text_renderer)
