#!/usr/bin/ruby -Eutf-8:utf-8
# encoding: UTF-8

require 'enc/encdb'
require 'enc/trans/transdb'
require 'certlint'

def runcablint(raw)
  m = CertLint::CABLint.lint(raw)
  m2 = []
  m.each do |msg|
    begin
      m2 << msg
    rescue Encoding::UndefinedConversionError
      m2 << "F: Encoding::UndefinedConversionError"
    end
  end
  return m2
end
