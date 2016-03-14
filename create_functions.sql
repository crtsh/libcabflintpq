CREATE OR REPLACE FUNCTION cablint_embedded(bytea) RETURNS SETOF text
	AS '$libdir/libcablintpq.so' LANGUAGE c IMMUTABLE;
