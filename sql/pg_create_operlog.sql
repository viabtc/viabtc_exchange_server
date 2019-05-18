--
-- PostgreSQL database dump
--

--
-- Name: operlog; Type: TABLE; Schema: public; Owner: matchengine
--

CREATE TABLE public.operlog (
    id bigint NOT NULL,
    query_date date DEFAULT now() NOT NULL,
    "time" timestamp with time zone DEFAULT now() NOT NULL,
    query jsonb
)
PARTITION BY RANGE (query_date);


ALTER TABLE public.operlog OWNER TO matchengine;

--
-- Name: slice_balance; Type: TABLE; Schema: public; Owner: matchengine
--

CREATE TABLE public.slice_balance (
    user_id bigint NOT NULL,
    asset character varying NOT NULL,
    balance_type integer NOT NULL,
    balance numeric(30,16) NOT NULL
);


ALTER TABLE public.slice_balance OWNER TO matchengine;

--
-- Name: slice_balance_1553058000; Type: TABLE; Schema: public; Owner: matchengine
--

CREATE TABLE public.slice_balance_1553058000 (
    user_id bigint NOT NULL,
    asset character varying NOT NULL,
    balance_type integer NOT NULL,
    balance numeric(30,16) NOT NULL,
    id integer NOT NULL
);


ALTER TABLE public.slice_balance_1553058000 OWNER TO matchengine;

--
-- Name: slice_balance_1553058000_id_seq; Type: SEQUENCE; Schema: public; Owner: matchengine
--

CREATE SEQUENCE public.slice_balance_1553058000_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.slice_balance_1553058000_id_seq OWNER TO matchengine;

--
-- Name: slice_balance_1553058000_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: matchengine
--

ALTER SEQUENCE public.slice_balance_1553058000_id_seq OWNED BY public.slice_balance_1553058000.id;


--
-- Name: slice_history; Type: TABLE; Schema: public; Owner: matchengine
--

CREATE TABLE public.slice_history (
    id integer NOT NULL,
    "time" timestamp with time zone NOT NULL,
    end_oper_id bigint NOT NULL,
    end_order_id bigint NOT NULL,
    end_deals_id bigint NOT NULL
);


ALTER TABLE public.slice_history OWNER TO matchengine;

--
-- Name: slice_history_id_seq; Type: SEQUENCE; Schema: public; Owner: matchengine
--

CREATE SEQUENCE public.slice_history_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.slice_history_id_seq OWNER TO matchengine;

--
-- Name: slice_history_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: matchengine
--

ALTER SEQUENCE public.slice_history_id_seq OWNED BY public.slice_history.id;


--
-- Name: slice_order; Type: TABLE; Schema: public; Owner: matchengine
--

CREATE TABLE public.slice_order (
    id bigint NOT NULL,
    order_type integer NOT NULL,
    side integer NOT NULL,
    create_time timestamp with time zone DEFAULT now() NOT NULL,
    update_time timestamp with time zone DEFAULT now() NOT NULL,
    user_id bigint NOT NULL,
    market character varying NOT NULL,
    price numeric(30,8) NOT NULL,
    amount numeric(30,8) NOT NULL,
    taker_fee numeric(30,8) NOT NULL,
    maker_fee numeric(30,8) NOT NULL,
    "left" numeric(30,8) NOT NULL,
    "freeze" numeric(30,8) NOT NULL,
    deal_stock numeric(30,8) NOT NULL,
    deal_money numeric(30,16) NOT NULL,
    deal_fee numeric(30,12) NOT NULL
);


ALTER TABLE public.slice_order OWNER TO matchengine;

--
-- Name: slice_order_1553058000; Type: TABLE; Schema: public; Owner: matchengine
--

CREATE TABLE public.slice_order_1553058000 (
    id bigint NOT NULL,
    order_type integer NOT NULL,
    side integer NOT NULL,
    create_time timestamp with time zone NOT NULL,
    update_time timestamp with time zone NOT NULL,
    user_id bigint NOT NULL,
    market character varying NOT NULL,
    price numeric(30,8) NOT NULL,
    amount numeric(30,8) NOT NULL,
    taker_fee numeric(30,8) NOT NULL,
    maker_fee numeric(30,8) NOT NULL,
    "left" numeric(30,8) NOT NULL,
    "freeze" numeric(30,8) NOT NULL,
    deal_stock numeric(30,8) NOT NULL,
    deal_money numeric(30,16) NOT NULL,
    deal_fee numeric(30,12) NOT NULL
);


ALTER TABLE public.slice_order_1553058000 OWNER TO matchengine;

--
-- Name: slice_balance_1553058000 id; Type: DEFAULT; Schema: public; Owner: matchengine
--

ALTER TABLE ONLY public.slice_balance_1553058000 ALTER COLUMN id SET DEFAULT nextval('public.slice_balance_1553058000_id_seq'::regclass);


--
-- Name: slice_history id; Type: DEFAULT; Schema: public; Owner: matchengine
--

ALTER TABLE ONLY public.slice_history ALTER COLUMN id SET DEFAULT nextval('public.slice_history_id_seq'::regclass);


--
-- Name: slice_balance_1553058000 slice_balance_1553058000_pkey; Type: CONSTRAINT; Schema: public; Owner: matchengine
--

ALTER TABLE ONLY public.slice_balance_1553058000
    ADD CONSTRAINT slice_balance_1553058000_pkey PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

