entity predef1 is
end entity;

architecture test of predef1 is
    type my_int is range 1 to 10;
    type my_enum is (X, Y, FOO, BAR);
    type int_vector is array (natural range <>) of integer;
    type my_char is ('a', 'b', 'c');
    type char_vector is array (natural range <>) of my_char;

    signal sa : bit := '0';
    signal sb : boolean := true;
begin

    main: process is
        variable b : boolean;
        variable i : integer;
        variable m : my_int;
        variable e : my_enum;
        variable r : real := 1.23456;
        variable v : int_vector(1 to 3) := (1, 2, 3);
        variable c : char_vector(1 to 3) := "bca";
    begin
        -----------------------------------------------------------------------
        -- MINIMUM

        assert minimum(1, 2) = 1;
        m := 5;
        b := true;
        wait for 1 ns;
        assert minimum(4, m) = 4;
        assert minimum(b, false) = false;
        assert minimum(int_vector'(5, -1, 2)) = -1;
        assert minimum(v) = 1;

        -----------------------------------------------------------------------
        -- MAXIMUM

        assert maximum(2, 1) = 2;
        m := 6;
        wait for 1 ns;
        assert maximum(4, m) = 6;
        assert maximum(b, false) = true;
        assert maximum(int_vector'(5, -1, 2)) = 5;
        assert maximum(v) = 3;

        -----------------------------------------------------------------------
        -- TO_STRING

        assert to_string(my_int'(7)) = "7";
        assert to_string(m) = "6";
        assert to_string(foo) = "foo";
        assert to_string(e) = my_enum'image(e);

        report to_string(now);
        assert to_string(now) = "2000000 fs";
        report to_string(now, ns);
        assert to_string(now, ns) = "2 ns";
        assert to_string(value => now, unit => hr) = "0 hr";

        report to_string(r, 2);
        assert to_string(r, 2) = "1.23";
        report to_string(r, 0);
        assert to_string(r, 0)(1 to 7) = "1.23456";

        report to_string(r, "%1.1f");
        assert to_string(r, "%1.1f") = "1.2";

        report to_string(char_vector'("abc"));
        assert to_string(char_vector'("abc")) = "abc";
        assert to_string(c) = "bca";

        -----------------------------------------------------------------------
        -- RISING_EDGE / FALLING_EDGE

        sa <= '1';
        sb <= false;
        wait for 0 ns;
        assert sa'event and sa = '1';
        assert rising_edge(sa);
        assert not falling_edge(sa);
        assert sb'event and sb = false;
        assert not rising_edge(sb);
        assert falling_edge(sb);

        wait;
    end process;

end architecture;