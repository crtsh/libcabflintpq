require 'certlint'

def runcablint(raw)
  return CertLint::CABLint.lint(raw)
end
