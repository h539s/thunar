test_bins = [
  'test-resolve-symlink',
]

test_screenshot_sources = [
  'test-screenshot-main.c',
  'thunar-test-utils.c',
]

e = executable(
  'test-screenshot',
  sources: test_screenshot_sources,
  c_args: thunar_c_args + ['-UG_DISABLE_ASSERT', '-DTHUNAR_TEST_SCREENSHOT'],
  include_directories: [
    include_directories('..'),
  ],
  dependencies: thunar_dependencies,
  link_with: [ libthunar, libthunarx ],
  install: false,
)

xvfb_run = find_program('xvfb-run', required: false)
dbus_run_session = find_program('dbus-run-session', required: false)

if xvfb_run.found() and dbus_run_session.found()
  foreach tc : ['open', 'file_menu']
    test(
      'test-screenshot-' + tc,
      xvfb_run,
      args: [
        '--auto-servernum',
        '-s', '-screen 0 800x600x24',
        dbus_run_session.full_path(),
        '--' , e.full_path()
      ],
      env: ['THUNAR_TEST_CASE=' + tc],
      timeout: 60,
    )
  endforeach
endif

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
    dependencies: thunar_dependencies,
    link_with: [ libthunar, libthunarx ],
    install: false,
  )

  test(bin, e)
endforeach
