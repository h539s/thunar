test_bins = [
  'test-resolve-symlink',
]

test_screenshot_common_sources = [
  'test-screenshot-main.c',
  'thunar-test-snapshots.c',
  'thunar-test-control.c',
]

test_cases = [
  'open',
  'file-menu',
]

xvfb_run = find_program('xvfb-run', required: false)
dbus_run_session = find_program('dbus-run-session', required: false)

foreach tc : test_cases
  tc_name = 'test-screenshot-' + tc
  e = executable(
    tc_name,
    sources: test_screenshot_common_sources + [tc_name + '.c'],
    c_args: thunar_c_args + ['-UG_DISABLE_ASSERT', '-DTHUNAR_TEST_SCREENSHOT'],
    include_directories: [
      include_directories('..'),
    ],
    dependencies: thunar_dependencies,
    link_with: [ libthunar, libthunarx ],
    install: false,
  )

  if xvfb_run.found() and dbus_run_session.found()
    test(
      tc_name,
      xvfb_run,
      args: [
        '--auto-servernum',
        '-s', '-screen 0 800x600x24',
        dbus_run_session.full_path(),
        '--' , e.full_path()
      ],
      timeout: 60,
    )
  endif
endforeach

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
