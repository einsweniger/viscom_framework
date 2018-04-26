#!/usr/bin/env python3

import Commands

def main():
  parser = Commands.get_parser()
  args, unknown = parser.parse_known_args()
  if 'func' in args:
    kwargs = vars(args)
    func = kwargs.pop('func')
    func(**kwargs)
  else:
    parser.print_help()


if __name__ == '__main__':
  try:
    main()
    print('\nexiting…')
  except KeyboardInterrupt:
    print('exiting…')
