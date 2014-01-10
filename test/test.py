#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

import sys
import socket


if __name__ == '__main__':
    if len(sys.argv) < 2:
        exit(1)
    if sys.argv[1].upper() in {'PP', 'KW', 'AW', 'DW', 'AD'}:
        try:
            action = sys.argv[1].upper()
            words = sys.argv[2].replace('#', '\t')
            # words = sys.argv[2]
            print (action, ' ', words, len('\t'), '\t')
            if len(words) <= 0:
                exit(1)
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
            sock.connect(('127.0.0.1', 9100))
            sock.send(bytes('{0}\r\n{1}'.format(action, words), 'utf-8'))
            a = sock.recv(1000)
            print (a)
            print (a.decode('utf-8'))
        finally:
            sock.close()
