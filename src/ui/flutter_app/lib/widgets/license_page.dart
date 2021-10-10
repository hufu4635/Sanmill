/*
  This file is part of Sanmill.
  Copyright (C) 2019-2021 The Sanmill developers (see AUTHORS file)

  Sanmill is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Sanmill is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import 'package:flutter/material.dart';
import 'package:flutter/services.dart' show rootBundle;
import 'package:sanmill/common/constants.dart';
import 'package:sanmill/generated/l10n.dart';

class LicenseAgreementPage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return FutureBuilder<String>(
      future: rootBundle.loadString(Constants.gplLicenseFilename),
      builder: (context, data) {
        late final String _data;
        if (!data.hasData) {
          _data = 'Nothing to show';
        } else {
          _data = data.data!;
        }

        return Scaffold(
          appBar: AppBar(title: Text(S.of(context).license), centerTitle: true),
          body: ListView(
            children: <Widget>[
              Container(
                padding: const EdgeInsets.only(
                  top: 16,
                  left: 16,
                  right: 16,
                  bottom: 16,
                ),
                child: Text(
                  _data,
                  style: const TextStyle(fontFamily: 'Monospace', fontSize: 12),
                  textAlign: TextAlign.left,
                ),
              ),
            ],
          ),
        );
      },
    );
  }
}
