require 'enc/encdb'
require 'enc/trans/transdb'
require 'certlint'

def runcablint(raw)
  return CertLint::CABLint.lint(raw)
end
