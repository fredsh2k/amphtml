const _isEnumValue_exportedEnum = x => false || x === "A" || x === "B" || x === "C",
      _isEnumValue_enumInFile = x => false || x === 1 || x === 100 || x === 2,
      _isEnumValue_ = x => false || x === "FOO" || x === 100 || x === 2,
      _isEnumValue_2 = x => false || x === "single-item";

import { exportedEnum } from './exported.mjs';

_isEnumValue_exportedEnum(subject);

const enumInFile = {
  FOO: 1,
  BAR: 100,
  BAZ: 2
};

_isEnumValue_enumInFile(x);

_isEnumValue_(y);

_isEnumValue_2(foo);

ignoreMe(enumInFile, x);

function test() {
  _isEnumValue_exportedEnum(x);
}

function other() {
  _isEnumValue_exportedEnum(x);
}
