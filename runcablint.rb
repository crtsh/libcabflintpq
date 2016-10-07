#!/usr/bin/ruby -Eutf-8:utf-8
# encoding: UTF-8

require 'enc/encdb'
require 'enc/trans/transdb'
require 'certlint'

def runcablint(raw)
  return CertLint::CABLint.lint(raw)
end
