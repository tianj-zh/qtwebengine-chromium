#!/usr/bin/python
# Copyright (c) 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Certificate chain with 1 intermediary and a trusted root. The intermediary
has an unknown X.509v3 extension that is marked as non-critical. Verification
is expected to succeed because although unrecognized, the extension is not
critical."""

import common

# Self-signed root certificate (part of trust store).
root = common.create_self_signed_root_certificate('Root')
intermediary = common.create_intermediary_certificate('Intermediary', root)

# Intermediary that has an unknown non-critical extension.
intermediary.get_extensions().add_property('1.2.3.4', 'DER:01:02:03:04')

# Target certificate.
target = common.create_end_entity_certificate('Target', intermediary)

chain = [target, intermediary]
trusted = [root]
time = common.DEFAULT_TIME
verify_result = True

common.write_test_file(__doc__, chain, trusted, time, verify_result)
