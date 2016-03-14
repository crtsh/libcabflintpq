# libcablintpq
Embed Ruby and run cablint from a PostgreSQL function.


Use a custom build of Ruby 2.0, because for some reason it crashes with Ruby 2.1+.

Login as root, then...
```
cd git
git clone https://github.com/ruby/ruby.git
cd ruby
git checkout ruby_2_0_0
autoconf
./configure --prefix=/usr/local/ruby_2_0_0 --enable-shared
make
make check
make install
RUBYOPT= PATH=/usr/local/ruby_2_0_0/bin:$PATH gem install open4
RUBYOPT= PATH=/usr/local/ruby_2_0_0/bin:$PATH gem install simpleidn
RUBYOPT= PATH=/usr/local/ruby_2_0_0/bin:$PATH gem install public_suffix
RUBYOPT= PATH=/usr/local/ruby_2_0_0/bin:$PATH gem install unf
```
