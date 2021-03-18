--
-- PostgreSQL database dump
--

--
-- Name: balance_history; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history (
    id bigint NOT NULL,
    "time" timestamp with time zone DEFAULT now() NOT NULL,
    user_id bigint NOT NULL,
    asset character varying NOT NULL,
    business character varying NOT NULL,
    change numeric(30,8) NOT NULL,
    balance numeric(30,16) NOT NULL,
    detail character varying NOT NULL
)
PARTITION BY LIST (((user_id % (100)::bigint)));


ALTER TABLE public.balance_history OWNER TO postgres;

--
-- Name: balance_history_0; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_0 PARTITION OF public.balance_history
FOR VALUES IN ('0');


ALTER TABLE public.balance_history_0 OWNER TO postgres;

--
-- Name: balance_history_1; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_1 PARTITION OF public.balance_history
FOR VALUES IN ('1');


ALTER TABLE public.balance_history_1 OWNER TO postgres;

--
-- Name: balance_history_10; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_10 PARTITION OF public.balance_history
FOR VALUES IN ('10');


ALTER TABLE public.balance_history_10 OWNER TO postgres;

--
-- Name: balance_history_11; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_11 PARTITION OF public.balance_history
FOR VALUES IN ('11');


ALTER TABLE public.balance_history_11 OWNER TO postgres;

--
-- Name: balance_history_12; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_12 PARTITION OF public.balance_history
FOR VALUES IN ('12');


ALTER TABLE public.balance_history_12 OWNER TO postgres;

--
-- Name: balance_history_13; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_13 PARTITION OF public.balance_history
FOR VALUES IN ('13');


ALTER TABLE public.balance_history_13 OWNER TO postgres;

--
-- Name: balance_history_14; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_14 PARTITION OF public.balance_history
FOR VALUES IN ('14');


ALTER TABLE public.balance_history_14 OWNER TO postgres;

--
-- Name: balance_history_15; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_15 PARTITION OF public.balance_history
FOR VALUES IN ('15');


ALTER TABLE public.balance_history_15 OWNER TO postgres;

--
-- Name: balance_history_16; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_16 PARTITION OF public.balance_history
FOR VALUES IN ('16');


ALTER TABLE public.balance_history_16 OWNER TO postgres;

--
-- Name: balance_history_17; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_17 PARTITION OF public.balance_history
FOR VALUES IN ('17');


ALTER TABLE public.balance_history_17 OWNER TO postgres;

--
-- Name: balance_history_18; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_18 PARTITION OF public.balance_history
FOR VALUES IN ('18');


ALTER TABLE public.balance_history_18 OWNER TO postgres;

--
-- Name: balance_history_19; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_19 PARTITION OF public.balance_history
FOR VALUES IN ('19');


ALTER TABLE public.balance_history_19 OWNER TO postgres;

--
-- Name: balance_history_2; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_2 PARTITION OF public.balance_history
FOR VALUES IN ('2');


ALTER TABLE public.balance_history_2 OWNER TO postgres;

--
-- Name: balance_history_20; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_20 PARTITION OF public.balance_history
FOR VALUES IN ('20');


ALTER TABLE public.balance_history_20 OWNER TO postgres;

--
-- Name: balance_history_21; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_21 PARTITION OF public.balance_history
FOR VALUES IN ('21');


ALTER TABLE public.balance_history_21 OWNER TO postgres;

--
-- Name: balance_history_22; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_22 PARTITION OF public.balance_history
FOR VALUES IN ('22');


ALTER TABLE public.balance_history_22 OWNER TO postgres;

--
-- Name: balance_history_23; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_23 PARTITION OF public.balance_history
FOR VALUES IN ('23');


ALTER TABLE public.balance_history_23 OWNER TO postgres;

--
-- Name: balance_history_24; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_24 PARTITION OF public.balance_history
FOR VALUES IN ('24');


ALTER TABLE public.balance_history_24 OWNER TO postgres;

--
-- Name: balance_history_25; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_25 PARTITION OF public.balance_history
FOR VALUES IN ('25');


ALTER TABLE public.balance_history_25 OWNER TO postgres;

--
-- Name: balance_history_26; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_26 PARTITION OF public.balance_history
FOR VALUES IN ('26');


ALTER TABLE public.balance_history_26 OWNER TO postgres;

--
-- Name: balance_history_27; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_27 PARTITION OF public.balance_history
FOR VALUES IN ('27');


ALTER TABLE public.balance_history_27 OWNER TO postgres;

--
-- Name: balance_history_28; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_28 PARTITION OF public.balance_history
FOR VALUES IN ('28');


ALTER TABLE public.balance_history_28 OWNER TO postgres;

--
-- Name: balance_history_29; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_29 PARTITION OF public.balance_history
FOR VALUES IN ('29');


ALTER TABLE public.balance_history_29 OWNER TO postgres;

--
-- Name: balance_history_3; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_3 PARTITION OF public.balance_history
FOR VALUES IN ('3');


ALTER TABLE public.balance_history_3 OWNER TO postgres;

--
-- Name: balance_history_30; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_30 PARTITION OF public.balance_history
FOR VALUES IN ('30');


ALTER TABLE public.balance_history_30 OWNER TO postgres;

--
-- Name: balance_history_31; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_31 PARTITION OF public.balance_history
FOR VALUES IN ('31');


ALTER TABLE public.balance_history_31 OWNER TO postgres;

--
-- Name: balance_history_32; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_32 PARTITION OF public.balance_history
FOR VALUES IN ('32');


ALTER TABLE public.balance_history_32 OWNER TO postgres;

--
-- Name: balance_history_33; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_33 PARTITION OF public.balance_history
FOR VALUES IN ('33');


ALTER TABLE public.balance_history_33 OWNER TO postgres;

--
-- Name: balance_history_34; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_34 PARTITION OF public.balance_history
FOR VALUES IN ('34');


ALTER TABLE public.balance_history_34 OWNER TO postgres;

--
-- Name: balance_history_35; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_35 PARTITION OF public.balance_history
FOR VALUES IN ('35');


ALTER TABLE public.balance_history_35 OWNER TO postgres;

--
-- Name: balance_history_36; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_36 PARTITION OF public.balance_history
FOR VALUES IN ('36');


ALTER TABLE public.balance_history_36 OWNER TO postgres;

--
-- Name: balance_history_37; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_37 PARTITION OF public.balance_history
FOR VALUES IN ('37');


ALTER TABLE public.balance_history_37 OWNER TO postgres;

--
-- Name: balance_history_38; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_38 PARTITION OF public.balance_history
FOR VALUES IN ('38');


ALTER TABLE public.balance_history_38 OWNER TO postgres;

--
-- Name: balance_history_39; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_39 PARTITION OF public.balance_history
FOR VALUES IN ('39');


ALTER TABLE public.balance_history_39 OWNER TO postgres;

--
-- Name: balance_history_4; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_4 PARTITION OF public.balance_history
FOR VALUES IN ('4');


ALTER TABLE public.balance_history_4 OWNER TO postgres;

--
-- Name: balance_history_40; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_40 PARTITION OF public.balance_history
FOR VALUES IN ('40');


ALTER TABLE public.balance_history_40 OWNER TO postgres;

--
-- Name: balance_history_41; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_41 PARTITION OF public.balance_history
FOR VALUES IN ('41');


ALTER TABLE public.balance_history_41 OWNER TO postgres;

--
-- Name: balance_history_42; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_42 PARTITION OF public.balance_history
FOR VALUES IN ('42');


ALTER TABLE public.balance_history_42 OWNER TO postgres;

--
-- Name: balance_history_43; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_43 PARTITION OF public.balance_history
FOR VALUES IN ('43');


ALTER TABLE public.balance_history_43 OWNER TO postgres;

--
-- Name: balance_history_44; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_44 PARTITION OF public.balance_history
FOR VALUES IN ('44');


ALTER TABLE public.balance_history_44 OWNER TO postgres;

--
-- Name: balance_history_45; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_45 PARTITION OF public.balance_history
FOR VALUES IN ('45');


ALTER TABLE public.balance_history_45 OWNER TO postgres;

--
-- Name: balance_history_46; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_46 PARTITION OF public.balance_history
FOR VALUES IN ('46');


ALTER TABLE public.balance_history_46 OWNER TO postgres;

--
-- Name: balance_history_47; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_47 PARTITION OF public.balance_history
FOR VALUES IN ('47');


ALTER TABLE public.balance_history_47 OWNER TO postgres;

--
-- Name: balance_history_48; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_48 PARTITION OF public.balance_history
FOR VALUES IN ('48');


ALTER TABLE public.balance_history_48 OWNER TO postgres;

--
-- Name: balance_history_49; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_49 PARTITION OF public.balance_history
FOR VALUES IN ('49');


ALTER TABLE public.balance_history_49 OWNER TO postgres;

--
-- Name: balance_history_5; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_5 PARTITION OF public.balance_history
FOR VALUES IN ('5');


ALTER TABLE public.balance_history_5 OWNER TO postgres;

--
-- Name: balance_history_50; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_50 PARTITION OF public.balance_history
FOR VALUES IN ('50');


ALTER TABLE public.balance_history_50 OWNER TO postgres;

--
-- Name: balance_history_51; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_51 PARTITION OF public.balance_history
FOR VALUES IN ('51');


ALTER TABLE public.balance_history_51 OWNER TO postgres;

--
-- Name: balance_history_52; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_52 PARTITION OF public.balance_history
FOR VALUES IN ('52');


ALTER TABLE public.balance_history_52 OWNER TO postgres;

--
-- Name: balance_history_53; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_53 PARTITION OF public.balance_history
FOR VALUES IN ('53');


ALTER TABLE public.balance_history_53 OWNER TO postgres;

--
-- Name: balance_history_54; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_54 PARTITION OF public.balance_history
FOR VALUES IN ('54');


ALTER TABLE public.balance_history_54 OWNER TO postgres;

--
-- Name: balance_history_55; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_55 PARTITION OF public.balance_history
FOR VALUES IN ('55');


ALTER TABLE public.balance_history_55 OWNER TO postgres;

--
-- Name: balance_history_56; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_56 PARTITION OF public.balance_history
FOR VALUES IN ('56');


ALTER TABLE public.balance_history_56 OWNER TO postgres;

--
-- Name: balance_history_57; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_57 PARTITION OF public.balance_history
FOR VALUES IN ('57');


ALTER TABLE public.balance_history_57 OWNER TO postgres;

--
-- Name: balance_history_58; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_58 PARTITION OF public.balance_history
FOR VALUES IN ('58');


ALTER TABLE public.balance_history_58 OWNER TO postgres;

--
-- Name: balance_history_59; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_59 PARTITION OF public.balance_history
FOR VALUES IN ('59');


ALTER TABLE public.balance_history_59 OWNER TO postgres;

--
-- Name: balance_history_6; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_6 PARTITION OF public.balance_history
FOR VALUES IN ('6');


ALTER TABLE public.balance_history_6 OWNER TO postgres;

--
-- Name: balance_history_60; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_60 PARTITION OF public.balance_history
FOR VALUES IN ('60');


ALTER TABLE public.balance_history_60 OWNER TO postgres;

--
-- Name: balance_history_61; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_61 PARTITION OF public.balance_history
FOR VALUES IN ('61');


ALTER TABLE public.balance_history_61 OWNER TO postgres;

--
-- Name: balance_history_62; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_62 PARTITION OF public.balance_history
FOR VALUES IN ('62');


ALTER TABLE public.balance_history_62 OWNER TO postgres;

--
-- Name: balance_history_63; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_63 PARTITION OF public.balance_history
FOR VALUES IN ('63');


ALTER TABLE public.balance_history_63 OWNER TO postgres;

--
-- Name: balance_history_64; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_64 PARTITION OF public.balance_history
FOR VALUES IN ('64');


ALTER TABLE public.balance_history_64 OWNER TO postgres;

--
-- Name: balance_history_65; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_65 PARTITION OF public.balance_history
FOR VALUES IN ('65');


ALTER TABLE public.balance_history_65 OWNER TO postgres;

--
-- Name: balance_history_66; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_66 PARTITION OF public.balance_history
FOR VALUES IN ('66');


ALTER TABLE public.balance_history_66 OWNER TO postgres;

--
-- Name: balance_history_67; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_67 PARTITION OF public.balance_history
FOR VALUES IN ('67');


ALTER TABLE public.balance_history_67 OWNER TO postgres;

--
-- Name: balance_history_68; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_68 PARTITION OF public.balance_history
FOR VALUES IN ('68');


ALTER TABLE public.balance_history_68 OWNER TO postgres;

--
-- Name: balance_history_69; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_69 PARTITION OF public.balance_history
FOR VALUES IN ('69');


ALTER TABLE public.balance_history_69 OWNER TO postgres;

--
-- Name: balance_history_7; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_7 PARTITION OF public.balance_history
FOR VALUES IN ('7');


ALTER TABLE public.balance_history_7 OWNER TO postgres;

--
-- Name: balance_history_70; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_70 PARTITION OF public.balance_history
FOR VALUES IN ('70');


ALTER TABLE public.balance_history_70 OWNER TO postgres;

--
-- Name: balance_history_71; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_71 PARTITION OF public.balance_history
FOR VALUES IN ('71');


ALTER TABLE public.balance_history_71 OWNER TO postgres;

--
-- Name: balance_history_72; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_72 PARTITION OF public.balance_history
FOR VALUES IN ('72');


ALTER TABLE public.balance_history_72 OWNER TO postgres;

--
-- Name: balance_history_73; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_73 PARTITION OF public.balance_history
FOR VALUES IN ('73');


ALTER TABLE public.balance_history_73 OWNER TO postgres;

--
-- Name: balance_history_74; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_74 PARTITION OF public.balance_history
FOR VALUES IN ('74');


ALTER TABLE public.balance_history_74 OWNER TO postgres;

--
-- Name: balance_history_75; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_75 PARTITION OF public.balance_history
FOR VALUES IN ('75');


ALTER TABLE public.balance_history_75 OWNER TO postgres;

--
-- Name: balance_history_76; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_76 PARTITION OF public.balance_history
FOR VALUES IN ('76');


ALTER TABLE public.balance_history_76 OWNER TO postgres;

--
-- Name: balance_history_77; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_77 PARTITION OF public.balance_history
FOR VALUES IN ('77');


ALTER TABLE public.balance_history_77 OWNER TO postgres;

--
-- Name: balance_history_78; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_78 PARTITION OF public.balance_history
FOR VALUES IN ('78');


ALTER TABLE public.balance_history_78 OWNER TO postgres;

--
-- Name: balance_history_79; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_79 PARTITION OF public.balance_history
FOR VALUES IN ('79');


ALTER TABLE public.balance_history_79 OWNER TO postgres;

--
-- Name: balance_history_8; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_8 PARTITION OF public.balance_history
FOR VALUES IN ('8');


ALTER TABLE public.balance_history_8 OWNER TO postgres;

--
-- Name: balance_history_80; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_80 PARTITION OF public.balance_history
FOR VALUES IN ('80');


ALTER TABLE public.balance_history_80 OWNER TO postgres;

--
-- Name: balance_history_81; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_81 PARTITION OF public.balance_history
FOR VALUES IN ('81');


ALTER TABLE public.balance_history_81 OWNER TO postgres;

--
-- Name: balance_history_82; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_82 PARTITION OF public.balance_history
FOR VALUES IN ('82');


ALTER TABLE public.balance_history_82 OWNER TO postgres;

--
-- Name: balance_history_83; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_83 PARTITION OF public.balance_history
FOR VALUES IN ('83');


ALTER TABLE public.balance_history_83 OWNER TO postgres;

--
-- Name: balance_history_84; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_84 PARTITION OF public.balance_history
FOR VALUES IN ('84');


ALTER TABLE public.balance_history_84 OWNER TO postgres;

--
-- Name: balance_history_85; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_85 PARTITION OF public.balance_history
FOR VALUES IN ('85');


ALTER TABLE public.balance_history_85 OWNER TO postgres;

--
-- Name: balance_history_86; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_86 PARTITION OF public.balance_history
FOR VALUES IN ('86');


ALTER TABLE public.balance_history_86 OWNER TO postgres;

--
-- Name: balance_history_87; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_87 PARTITION OF public.balance_history
FOR VALUES IN ('87');


ALTER TABLE public.balance_history_87 OWNER TO postgres;

--
-- Name: balance_history_88; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_88 PARTITION OF public.balance_history
FOR VALUES IN ('88');


ALTER TABLE public.balance_history_88 OWNER TO postgres;

--
-- Name: balance_history_89; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_89 PARTITION OF public.balance_history
FOR VALUES IN ('89');


ALTER TABLE public.balance_history_89 OWNER TO postgres;

--
-- Name: balance_history_9; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_9 PARTITION OF public.balance_history
FOR VALUES IN ('9');


ALTER TABLE public.balance_history_9 OWNER TO postgres;

--
-- Name: balance_history_90; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_90 PARTITION OF public.balance_history
FOR VALUES IN ('90');


ALTER TABLE public.balance_history_90 OWNER TO postgres;

--
-- Name: balance_history_91; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_91 PARTITION OF public.balance_history
FOR VALUES IN ('91');


ALTER TABLE public.balance_history_91 OWNER TO postgres;

--
-- Name: balance_history_92; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_92 PARTITION OF public.balance_history
FOR VALUES IN ('92');


ALTER TABLE public.balance_history_92 OWNER TO postgres;

--
-- Name: balance_history_93; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_93 PARTITION OF public.balance_history
FOR VALUES IN ('93');


ALTER TABLE public.balance_history_93 OWNER TO postgres;

--
-- Name: balance_history_94; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_94 PARTITION OF public.balance_history
FOR VALUES IN ('94');


ALTER TABLE public.balance_history_94 OWNER TO postgres;

--
-- Name: balance_history_95; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_95 PARTITION OF public.balance_history
FOR VALUES IN ('95');


ALTER TABLE public.balance_history_95 OWNER TO postgres;

--
-- Name: balance_history_96; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_96 PARTITION OF public.balance_history
FOR VALUES IN ('96');


ALTER TABLE public.balance_history_96 OWNER TO postgres;

--
-- Name: balance_history_97; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_97 PARTITION OF public.balance_history
FOR VALUES IN ('97');


ALTER TABLE public.balance_history_97 OWNER TO postgres;

--
-- Name: balance_history_98; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_98 PARTITION OF public.balance_history
FOR VALUES IN ('98');


ALTER TABLE public.balance_history_98 OWNER TO postgres;

--
-- Name: balance_history_99; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.balance_history_99 PARTITION OF public.balance_history
FOR VALUES IN ('99');


ALTER TABLE public.balance_history_99 OWNER TO postgres;

--
-- Name: balance_history_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.balance_history ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.balance_history_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: deal_history; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history (
    id bigint NOT NULL,
    "time" timestamp with time zone DEFAULT now() NOT NULL,
    user_id bigint NOT NULL,
    deal_id bigint NOT NULL,
    order_id bigint NOT NULL,
    deal_order_id bigint NOT NULL,
    role integer NOT NULL,
    price numeric(30,8) NOT NULL,
    amount numeric(30,8) NOT NULL,
    deal numeric(30,16) NOT NULL,
    fee numeric(30,16) NOT NULL,
    deal_fee numeric(30,16) NOT NULL
)
PARTITION BY LIST (((order_id % (100)::bigint)));


ALTER TABLE public.deal_history OWNER TO postgres;

--
-- Name: deal_history_0; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_0 PARTITION OF public.deal_history
FOR VALUES IN ('0');


ALTER TABLE public.deal_history_0 OWNER TO postgres;

--
-- Name: deal_history_1; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_1 PARTITION OF public.deal_history
FOR VALUES IN ('1');


ALTER TABLE public.deal_history_1 OWNER TO postgres;

--
-- Name: deal_history_10; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_10 PARTITION OF public.deal_history
FOR VALUES IN ('10');


ALTER TABLE public.deal_history_10 OWNER TO postgres;

--
-- Name: deal_history_11; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_11 PARTITION OF public.deal_history
FOR VALUES IN ('11');


ALTER TABLE public.deal_history_11 OWNER TO postgres;

--
-- Name: deal_history_12; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_12 PARTITION OF public.deal_history
FOR VALUES IN ('12');


ALTER TABLE public.deal_history_12 OWNER TO postgres;

--
-- Name: deal_history_13; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_13 PARTITION OF public.deal_history
FOR VALUES IN ('13');


ALTER TABLE public.deal_history_13 OWNER TO postgres;

--
-- Name: deal_history_14; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_14 PARTITION OF public.deal_history
FOR VALUES IN ('14');


ALTER TABLE public.deal_history_14 OWNER TO postgres;

--
-- Name: deal_history_15; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_15 PARTITION OF public.deal_history
FOR VALUES IN ('15');


ALTER TABLE public.deal_history_15 OWNER TO postgres;

--
-- Name: deal_history_16; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_16 PARTITION OF public.deal_history
FOR VALUES IN ('16');


ALTER TABLE public.deal_history_16 OWNER TO postgres;

--
-- Name: deal_history_17; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_17 PARTITION OF public.deal_history
FOR VALUES IN ('17');


ALTER TABLE public.deal_history_17 OWNER TO postgres;

--
-- Name: deal_history_18; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_18 PARTITION OF public.deal_history
FOR VALUES IN ('18');


ALTER TABLE public.deal_history_18 OWNER TO postgres;

--
-- Name: deal_history_19; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_19 PARTITION OF public.deal_history
FOR VALUES IN ('19');


ALTER TABLE public.deal_history_19 OWNER TO postgres;

--
-- Name: deal_history_2; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_2 PARTITION OF public.deal_history
FOR VALUES IN ('2');


ALTER TABLE public.deal_history_2 OWNER TO postgres;

--
-- Name: deal_history_20; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_20 PARTITION OF public.deal_history
FOR VALUES IN ('20');


ALTER TABLE public.deal_history_20 OWNER TO postgres;

--
-- Name: deal_history_21; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_21 PARTITION OF public.deal_history
FOR VALUES IN ('21');


ALTER TABLE public.deal_history_21 OWNER TO postgres;

--
-- Name: deal_history_22; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_22 PARTITION OF public.deal_history
FOR VALUES IN ('22');


ALTER TABLE public.deal_history_22 OWNER TO postgres;

--
-- Name: deal_history_23; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_23 PARTITION OF public.deal_history
FOR VALUES IN ('23');


ALTER TABLE public.deal_history_23 OWNER TO postgres;

--
-- Name: deal_history_24; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_24 PARTITION OF public.deal_history
FOR VALUES IN ('24');


ALTER TABLE public.deal_history_24 OWNER TO postgres;

--
-- Name: deal_history_25; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_25 PARTITION OF public.deal_history
FOR VALUES IN ('25');


ALTER TABLE public.deal_history_25 OWNER TO postgres;

--
-- Name: deal_history_26; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_26 PARTITION OF public.deal_history
FOR VALUES IN ('26');


ALTER TABLE public.deal_history_26 OWNER TO postgres;

--
-- Name: deal_history_27; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_27 PARTITION OF public.deal_history
FOR VALUES IN ('27');


ALTER TABLE public.deal_history_27 OWNER TO postgres;

--
-- Name: deal_history_28; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_28 PARTITION OF public.deal_history
FOR VALUES IN ('28');


ALTER TABLE public.deal_history_28 OWNER TO postgres;

--
-- Name: deal_history_29; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_29 PARTITION OF public.deal_history
FOR VALUES IN ('29');


ALTER TABLE public.deal_history_29 OWNER TO postgres;

--
-- Name: deal_history_3; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_3 PARTITION OF public.deal_history
FOR VALUES IN ('3');


ALTER TABLE public.deal_history_3 OWNER TO postgres;

--
-- Name: deal_history_30; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_30 PARTITION OF public.deal_history
FOR VALUES IN ('30');


ALTER TABLE public.deal_history_30 OWNER TO postgres;

--
-- Name: deal_history_31; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_31 PARTITION OF public.deal_history
FOR VALUES IN ('31');


ALTER TABLE public.deal_history_31 OWNER TO postgres;

--
-- Name: deal_history_32; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_32 PARTITION OF public.deal_history
FOR VALUES IN ('32');


ALTER TABLE public.deal_history_32 OWNER TO postgres;

--
-- Name: deal_history_33; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_33 PARTITION OF public.deal_history
FOR VALUES IN ('33');


ALTER TABLE public.deal_history_33 OWNER TO postgres;

--
-- Name: deal_history_34; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_34 PARTITION OF public.deal_history
FOR VALUES IN ('34');


ALTER TABLE public.deal_history_34 OWNER TO postgres;

--
-- Name: deal_history_35; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_35 PARTITION OF public.deal_history
FOR VALUES IN ('35');


ALTER TABLE public.deal_history_35 OWNER TO postgres;

--
-- Name: deal_history_36; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_36 PARTITION OF public.deal_history
FOR VALUES IN ('36');


ALTER TABLE public.deal_history_36 OWNER TO postgres;

--
-- Name: deal_history_37; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_37 PARTITION OF public.deal_history
FOR VALUES IN ('37');


ALTER TABLE public.deal_history_37 OWNER TO postgres;

--
-- Name: deal_history_38; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_38 PARTITION OF public.deal_history
FOR VALUES IN ('38');


ALTER TABLE public.deal_history_38 OWNER TO postgres;

--
-- Name: deal_history_39; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_39 PARTITION OF public.deal_history
FOR VALUES IN ('39');


ALTER TABLE public.deal_history_39 OWNER TO postgres;

--
-- Name: deal_history_4; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_4 PARTITION OF public.deal_history
FOR VALUES IN ('4');


ALTER TABLE public.deal_history_4 OWNER TO postgres;

--
-- Name: deal_history_40; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_40 PARTITION OF public.deal_history
FOR VALUES IN ('40');


ALTER TABLE public.deal_history_40 OWNER TO postgres;

--
-- Name: deal_history_41; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_41 PARTITION OF public.deal_history
FOR VALUES IN ('41');


ALTER TABLE public.deal_history_41 OWNER TO postgres;

--
-- Name: deal_history_42; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_42 PARTITION OF public.deal_history
FOR VALUES IN ('42');


ALTER TABLE public.deal_history_42 OWNER TO postgres;

--
-- Name: deal_history_43; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_43 PARTITION OF public.deal_history
FOR VALUES IN ('43');


ALTER TABLE public.deal_history_43 OWNER TO postgres;

--
-- Name: deal_history_44; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_44 PARTITION OF public.deal_history
FOR VALUES IN ('44');


ALTER TABLE public.deal_history_44 OWNER TO postgres;

--
-- Name: deal_history_45; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_45 PARTITION OF public.deal_history
FOR VALUES IN ('45');


ALTER TABLE public.deal_history_45 OWNER TO postgres;

--
-- Name: deal_history_46; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_46 PARTITION OF public.deal_history
FOR VALUES IN ('46');


ALTER TABLE public.deal_history_46 OWNER TO postgres;

--
-- Name: deal_history_47; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_47 PARTITION OF public.deal_history
FOR VALUES IN ('47');


ALTER TABLE public.deal_history_47 OWNER TO postgres;

--
-- Name: deal_history_48; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_48 PARTITION OF public.deal_history
FOR VALUES IN ('48');


ALTER TABLE public.deal_history_48 OWNER TO postgres;

--
-- Name: deal_history_49; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_49 PARTITION OF public.deal_history
FOR VALUES IN ('49');


ALTER TABLE public.deal_history_49 OWNER TO postgres;

--
-- Name: deal_history_5; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_5 PARTITION OF public.deal_history
FOR VALUES IN ('5');


ALTER TABLE public.deal_history_5 OWNER TO postgres;

--
-- Name: deal_history_50; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_50 PARTITION OF public.deal_history
FOR VALUES IN ('50');


ALTER TABLE public.deal_history_50 OWNER TO postgres;

--
-- Name: deal_history_51; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_51 PARTITION OF public.deal_history
FOR VALUES IN ('51');


ALTER TABLE public.deal_history_51 OWNER TO postgres;

--
-- Name: deal_history_52; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_52 PARTITION OF public.deal_history
FOR VALUES IN ('52');


ALTER TABLE public.deal_history_52 OWNER TO postgres;

--
-- Name: deal_history_53; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_53 PARTITION OF public.deal_history
FOR VALUES IN ('53');


ALTER TABLE public.deal_history_53 OWNER TO postgres;

--
-- Name: deal_history_54; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_54 PARTITION OF public.deal_history
FOR VALUES IN ('54');


ALTER TABLE public.deal_history_54 OWNER TO postgres;

--
-- Name: deal_history_55; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_55 PARTITION OF public.deal_history
FOR VALUES IN ('55');


ALTER TABLE public.deal_history_55 OWNER TO postgres;

--
-- Name: deal_history_56; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_56 PARTITION OF public.deal_history
FOR VALUES IN ('56');


ALTER TABLE public.deal_history_56 OWNER TO postgres;

--
-- Name: deal_history_57; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_57 PARTITION OF public.deal_history
FOR VALUES IN ('57');


ALTER TABLE public.deal_history_57 OWNER TO postgres;

--
-- Name: deal_history_58; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_58 PARTITION OF public.deal_history
FOR VALUES IN ('58');


ALTER TABLE public.deal_history_58 OWNER TO postgres;

--
-- Name: deal_history_59; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_59 PARTITION OF public.deal_history
FOR VALUES IN ('59');


ALTER TABLE public.deal_history_59 OWNER TO postgres;

--
-- Name: deal_history_6; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_6 PARTITION OF public.deal_history
FOR VALUES IN ('6');


ALTER TABLE public.deal_history_6 OWNER TO postgres;

--
-- Name: deal_history_60; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_60 PARTITION OF public.deal_history
FOR VALUES IN ('60');


ALTER TABLE public.deal_history_60 OWNER TO postgres;

--
-- Name: deal_history_61; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_61 PARTITION OF public.deal_history
FOR VALUES IN ('61');


ALTER TABLE public.deal_history_61 OWNER TO postgres;

--
-- Name: deal_history_62; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_62 PARTITION OF public.deal_history
FOR VALUES IN ('62');


ALTER TABLE public.deal_history_62 OWNER TO postgres;

--
-- Name: deal_history_63; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_63 PARTITION OF public.deal_history
FOR VALUES IN ('63');


ALTER TABLE public.deal_history_63 OWNER TO postgres;

--
-- Name: deal_history_64; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_64 PARTITION OF public.deal_history
FOR VALUES IN ('64');


ALTER TABLE public.deal_history_64 OWNER TO postgres;

--
-- Name: deal_history_65; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_65 PARTITION OF public.deal_history
FOR VALUES IN ('65');


ALTER TABLE public.deal_history_65 OWNER TO postgres;

--
-- Name: deal_history_66; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_66 PARTITION OF public.deal_history
FOR VALUES IN ('66');


ALTER TABLE public.deal_history_66 OWNER TO postgres;

--
-- Name: deal_history_67; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_67 PARTITION OF public.deal_history
FOR VALUES IN ('67');


ALTER TABLE public.deal_history_67 OWNER TO postgres;

--
-- Name: deal_history_68; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_68 PARTITION OF public.deal_history
FOR VALUES IN ('68');


ALTER TABLE public.deal_history_68 OWNER TO postgres;

--
-- Name: deal_history_69; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_69 PARTITION OF public.deal_history
FOR VALUES IN ('69');


ALTER TABLE public.deal_history_69 OWNER TO postgres;

--
-- Name: deal_history_7; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_7 PARTITION OF public.deal_history
FOR VALUES IN ('7');


ALTER TABLE public.deal_history_7 OWNER TO postgres;

--
-- Name: deal_history_70; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_70 PARTITION OF public.deal_history
FOR VALUES IN ('70');


ALTER TABLE public.deal_history_70 OWNER TO postgres;

--
-- Name: deal_history_71; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_71 PARTITION OF public.deal_history
FOR VALUES IN ('71');


ALTER TABLE public.deal_history_71 OWNER TO postgres;

--
-- Name: deal_history_72; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_72 PARTITION OF public.deal_history
FOR VALUES IN ('72');


ALTER TABLE public.deal_history_72 OWNER TO postgres;

--
-- Name: deal_history_73; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_73 PARTITION OF public.deal_history
FOR VALUES IN ('73');


ALTER TABLE public.deal_history_73 OWNER TO postgres;

--
-- Name: deal_history_74; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_74 PARTITION OF public.deal_history
FOR VALUES IN ('74');


ALTER TABLE public.deal_history_74 OWNER TO postgres;

--
-- Name: deal_history_75; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_75 PARTITION OF public.deal_history
FOR VALUES IN ('75');


ALTER TABLE public.deal_history_75 OWNER TO postgres;

--
-- Name: deal_history_76; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_76 PARTITION OF public.deal_history
FOR VALUES IN ('76');


ALTER TABLE public.deal_history_76 OWNER TO postgres;

--
-- Name: deal_history_77; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_77 PARTITION OF public.deal_history
FOR VALUES IN ('77');


ALTER TABLE public.deal_history_77 OWNER TO postgres;

--
-- Name: deal_history_78; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_78 PARTITION OF public.deal_history
FOR VALUES IN ('78');


ALTER TABLE public.deal_history_78 OWNER TO postgres;

--
-- Name: deal_history_79; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_79 PARTITION OF public.deal_history
FOR VALUES IN ('79');


ALTER TABLE public.deal_history_79 OWNER TO postgres;

--
-- Name: deal_history_8; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_8 PARTITION OF public.deal_history
FOR VALUES IN ('8');


ALTER TABLE public.deal_history_8 OWNER TO postgres;

--
-- Name: deal_history_80; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_80 PARTITION OF public.deal_history
FOR VALUES IN ('80');


ALTER TABLE public.deal_history_80 OWNER TO postgres;

--
-- Name: deal_history_81; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_81 PARTITION OF public.deal_history
FOR VALUES IN ('81');


ALTER TABLE public.deal_history_81 OWNER TO postgres;

--
-- Name: deal_history_82; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_82 PARTITION OF public.deal_history
FOR VALUES IN ('82');


ALTER TABLE public.deal_history_82 OWNER TO postgres;

--
-- Name: deal_history_83; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_83 PARTITION OF public.deal_history
FOR VALUES IN ('83');


ALTER TABLE public.deal_history_83 OWNER TO postgres;

--
-- Name: deal_history_84; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_84 PARTITION OF public.deal_history
FOR VALUES IN ('84');


ALTER TABLE public.deal_history_84 OWNER TO postgres;

--
-- Name: deal_history_85; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_85 PARTITION OF public.deal_history
FOR VALUES IN ('85');


ALTER TABLE public.deal_history_85 OWNER TO postgres;

--
-- Name: deal_history_86; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_86 PARTITION OF public.deal_history
FOR VALUES IN ('86');


ALTER TABLE public.deal_history_86 OWNER TO postgres;

--
-- Name: deal_history_87; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_87 PARTITION OF public.deal_history
FOR VALUES IN ('87');


ALTER TABLE public.deal_history_87 OWNER TO postgres;

--
-- Name: deal_history_88; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_88 PARTITION OF public.deal_history
FOR VALUES IN ('88');


ALTER TABLE public.deal_history_88 OWNER TO postgres;

--
-- Name: deal_history_89; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_89 PARTITION OF public.deal_history
FOR VALUES IN ('89');


ALTER TABLE public.deal_history_89 OWNER TO postgres;

--
-- Name: deal_history_9; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_9 PARTITION OF public.deal_history
FOR VALUES IN ('9');


ALTER TABLE public.deal_history_9 OWNER TO postgres;

--
-- Name: deal_history_90; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_90 PARTITION OF public.deal_history
FOR VALUES IN ('90');


ALTER TABLE public.deal_history_90 OWNER TO postgres;

--
-- Name: deal_history_91; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_91 PARTITION OF public.deal_history
FOR VALUES IN ('91');


ALTER TABLE public.deal_history_91 OWNER TO postgres;

--
-- Name: deal_history_92; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_92 PARTITION OF public.deal_history
FOR VALUES IN ('92');


ALTER TABLE public.deal_history_92 OWNER TO postgres;

--
-- Name: deal_history_93; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_93 PARTITION OF public.deal_history
FOR VALUES IN ('93');


ALTER TABLE public.deal_history_93 OWNER TO postgres;

--
-- Name: deal_history_94; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_94 PARTITION OF public.deal_history
FOR VALUES IN ('94');


ALTER TABLE public.deal_history_94 OWNER TO postgres;

--
-- Name: deal_history_95; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_95 PARTITION OF public.deal_history
FOR VALUES IN ('95');


ALTER TABLE public.deal_history_95 OWNER TO postgres;

--
-- Name: deal_history_96; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_96 PARTITION OF public.deal_history
FOR VALUES IN ('96');


ALTER TABLE public.deal_history_96 OWNER TO postgres;

--
-- Name: deal_history_97; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_97 PARTITION OF public.deal_history
FOR VALUES IN ('97');


ALTER TABLE public.deal_history_97 OWNER TO postgres;

--
-- Name: deal_history_98; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_98 PARTITION OF public.deal_history
FOR VALUES IN ('98');


ALTER TABLE public.deal_history_98 OWNER TO postgres;

--
-- Name: deal_history_99; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.deal_history_99 PARTITION OF public.deal_history
FOR VALUES IN ('99');


ALTER TABLE public.deal_history_99 OWNER TO postgres;

--
-- Name: deal_history_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.deal_history ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.deal_history_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);




--
-- Name: order_detail; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail (
    id bigint NOT NULL,
    create_time timestamp with time zone DEFAULT now() NOT NULL,
    finish_time timestamp with time zone DEFAULT now() NOT NULL,
    user_id bigint NOT NULL,
    market character varying NOT NULL,
    source character varying NOT NULL,
    order_type integer NOT NULL,
    side integer NOT NULL,
    price numeric(30,8) NOT NULL,
    amount numeric(30,8) NOT NULL,
    taker_fee numeric(30,8) NOT NULL,
    maker_fee numeric(30,8) NOT NULL,
    deal_stock numeric(30,8) NOT NULL,
    deal_money numeric(30,16) NOT NULL,
    deal_fee numeric(30,16) NOT NULL
)
PARTITION BY LIST (((id % (100)::bigint)));


ALTER TABLE public.order_detail OWNER TO postgres;

--
-- Name: order_detail_0; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_0 PARTITION OF public.order_detail
FOR VALUES IN ('0');


ALTER TABLE public.order_detail_0 OWNER TO postgres;

--
-- Name: order_detail_1; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_1 PARTITION OF public.order_detail
FOR VALUES IN ('1');


ALTER TABLE public.order_detail_1 OWNER TO postgres;

--
-- Name: order_detail_10; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_10 PARTITION OF public.order_detail
FOR VALUES IN ('10');


ALTER TABLE public.order_detail_10 OWNER TO postgres;

--
-- Name: order_detail_11; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_11 PARTITION OF public.order_detail
FOR VALUES IN ('11');


ALTER TABLE public.order_detail_11 OWNER TO postgres;

--
-- Name: order_detail_12; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_12 PARTITION OF public.order_detail
FOR VALUES IN ('12');


ALTER TABLE public.order_detail_12 OWNER TO postgres;

--
-- Name: order_detail_13; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_13 PARTITION OF public.order_detail
FOR VALUES IN ('13');


ALTER TABLE public.order_detail_13 OWNER TO postgres;

--
-- Name: order_detail_14; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_14 PARTITION OF public.order_detail
FOR VALUES IN ('14');


ALTER TABLE public.order_detail_14 OWNER TO postgres;

--
-- Name: order_detail_15; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_15 PARTITION OF public.order_detail
FOR VALUES IN ('15');


ALTER TABLE public.order_detail_15 OWNER TO postgres;

--
-- Name: order_detail_16; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_16 PARTITION OF public.order_detail
FOR VALUES IN ('16');


ALTER TABLE public.order_detail_16 OWNER TO postgres;

--
-- Name: order_detail_17; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_17 PARTITION OF public.order_detail
FOR VALUES IN ('17');


ALTER TABLE public.order_detail_17 OWNER TO postgres;

--
-- Name: order_detail_18; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_18 PARTITION OF public.order_detail
FOR VALUES IN ('18');


ALTER TABLE public.order_detail_18 OWNER TO postgres;

--
-- Name: order_detail_19; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_19 PARTITION OF public.order_detail
FOR VALUES IN ('19');


ALTER TABLE public.order_detail_19 OWNER TO postgres;

--
-- Name: order_detail_2; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_2 PARTITION OF public.order_detail
FOR VALUES IN ('2');


ALTER TABLE public.order_detail_2 OWNER TO postgres;

--
-- Name: order_detail_20; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_20 PARTITION OF public.order_detail
FOR VALUES IN ('20');


ALTER TABLE public.order_detail_20 OWNER TO postgres;

--
-- Name: order_detail_21; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_21 PARTITION OF public.order_detail
FOR VALUES IN ('21');


ALTER TABLE public.order_detail_21 OWNER TO postgres;

--
-- Name: order_detail_22; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_22 PARTITION OF public.order_detail
FOR VALUES IN ('22');


ALTER TABLE public.order_detail_22 OWNER TO postgres;

--
-- Name: order_detail_23; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_23 PARTITION OF public.order_detail
FOR VALUES IN ('23');


ALTER TABLE public.order_detail_23 OWNER TO postgres;

--
-- Name: order_detail_24; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_24 PARTITION OF public.order_detail
FOR VALUES IN ('24');


ALTER TABLE public.order_detail_24 OWNER TO postgres;

--
-- Name: order_detail_25; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_25 PARTITION OF public.order_detail
FOR VALUES IN ('25');


ALTER TABLE public.order_detail_25 OWNER TO postgres;

--
-- Name: order_detail_26; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_26 PARTITION OF public.order_detail
FOR VALUES IN ('26');


ALTER TABLE public.order_detail_26 OWNER TO postgres;

--
-- Name: order_detail_27; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_27 PARTITION OF public.order_detail
FOR VALUES IN ('27');


ALTER TABLE public.order_detail_27 OWNER TO postgres;

--
-- Name: order_detail_28; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_28 PARTITION OF public.order_detail
FOR VALUES IN ('28');


ALTER TABLE public.order_detail_28 OWNER TO postgres;

--
-- Name: order_detail_29; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_29 PARTITION OF public.order_detail
FOR VALUES IN ('29');


ALTER TABLE public.order_detail_29 OWNER TO postgres;

--
-- Name: order_detail_3; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_3 PARTITION OF public.order_detail
FOR VALUES IN ('3');


ALTER TABLE public.order_detail_3 OWNER TO postgres;

--
-- Name: order_detail_30; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_30 PARTITION OF public.order_detail
FOR VALUES IN ('30');


ALTER TABLE public.order_detail_30 OWNER TO postgres;

--
-- Name: order_detail_31; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_31 PARTITION OF public.order_detail
FOR VALUES IN ('31');


ALTER TABLE public.order_detail_31 OWNER TO postgres;

--
-- Name: order_detail_32; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_32 PARTITION OF public.order_detail
FOR VALUES IN ('32');


ALTER TABLE public.order_detail_32 OWNER TO postgres;

--
-- Name: order_detail_33; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_33 PARTITION OF public.order_detail
FOR VALUES IN ('33');


ALTER TABLE public.order_detail_33 OWNER TO postgres;

--
-- Name: order_detail_34; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_34 PARTITION OF public.order_detail
FOR VALUES IN ('34');


ALTER TABLE public.order_detail_34 OWNER TO postgres;

--
-- Name: order_detail_35; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_35 PARTITION OF public.order_detail
FOR VALUES IN ('35');


ALTER TABLE public.order_detail_35 OWNER TO postgres;

--
-- Name: order_detail_36; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_36 PARTITION OF public.order_detail
FOR VALUES IN ('36');


ALTER TABLE public.order_detail_36 OWNER TO postgres;

--
-- Name: order_detail_37; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_37 PARTITION OF public.order_detail
FOR VALUES IN ('37');


ALTER TABLE public.order_detail_37 OWNER TO postgres;

--
-- Name: order_detail_38; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_38 PARTITION OF public.order_detail
FOR VALUES IN ('38');


ALTER TABLE public.order_detail_38 OWNER TO postgres;

--
-- Name: order_detail_39; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_39 PARTITION OF public.order_detail
FOR VALUES IN ('39');


ALTER TABLE public.order_detail_39 OWNER TO postgres;

--
-- Name: order_detail_4; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_4 PARTITION OF public.order_detail
FOR VALUES IN ('4');


ALTER TABLE public.order_detail_4 OWNER TO postgres;

--
-- Name: order_detail_40; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_40 PARTITION OF public.order_detail
FOR VALUES IN ('40');


ALTER TABLE public.order_detail_40 OWNER TO postgres;

--
-- Name: order_detail_41; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_41 PARTITION OF public.order_detail
FOR VALUES IN ('41');


ALTER TABLE public.order_detail_41 OWNER TO postgres;

--
-- Name: order_detail_42; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_42 PARTITION OF public.order_detail
FOR VALUES IN ('42');


ALTER TABLE public.order_detail_42 OWNER TO postgres;

--
-- Name: order_detail_43; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_43 PARTITION OF public.order_detail
FOR VALUES IN ('43');


ALTER TABLE public.order_detail_43 OWNER TO postgres;

--
-- Name: order_detail_44; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_44 PARTITION OF public.order_detail
FOR VALUES IN ('44');


ALTER TABLE public.order_detail_44 OWNER TO postgres;

--
-- Name: order_detail_45; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_45 PARTITION OF public.order_detail
FOR VALUES IN ('45');


ALTER TABLE public.order_detail_45 OWNER TO postgres;

--
-- Name: order_detail_46; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_46 PARTITION OF public.order_detail
FOR VALUES IN ('46');


ALTER TABLE public.order_detail_46 OWNER TO postgres;

--
-- Name: order_detail_47; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_47 PARTITION OF public.order_detail
FOR VALUES IN ('47');


ALTER TABLE public.order_detail_47 OWNER TO postgres;

--
-- Name: order_detail_48; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_48 PARTITION OF public.order_detail
FOR VALUES IN ('48');


ALTER TABLE public.order_detail_48 OWNER TO postgres;

--
-- Name: order_detail_49; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_49 PARTITION OF public.order_detail
FOR VALUES IN ('49');


ALTER TABLE public.order_detail_49 OWNER TO postgres;

--
-- Name: order_detail_5; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_5 PARTITION OF public.order_detail
FOR VALUES IN ('5');


ALTER TABLE public.order_detail_5 OWNER TO postgres;

--
-- Name: order_detail_50; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_50 PARTITION OF public.order_detail
FOR VALUES IN ('50');


ALTER TABLE public.order_detail_50 OWNER TO postgres;

--
-- Name: order_detail_51; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_51 PARTITION OF public.order_detail
FOR VALUES IN ('51');


ALTER TABLE public.order_detail_51 OWNER TO postgres;

--
-- Name: order_detail_52; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_52 PARTITION OF public.order_detail
FOR VALUES IN ('52');


ALTER TABLE public.order_detail_52 OWNER TO postgres;

--
-- Name: order_detail_53; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_53 PARTITION OF public.order_detail
FOR VALUES IN ('53');


ALTER TABLE public.order_detail_53 OWNER TO postgres;

--
-- Name: order_detail_54; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_54 PARTITION OF public.order_detail
FOR VALUES IN ('54');


ALTER TABLE public.order_detail_54 OWNER TO postgres;

--
-- Name: order_detail_55; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_55 PARTITION OF public.order_detail
FOR VALUES IN ('55');


ALTER TABLE public.order_detail_55 OWNER TO postgres;

--
-- Name: order_detail_56; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_56 PARTITION OF public.order_detail
FOR VALUES IN ('56');


ALTER TABLE public.order_detail_56 OWNER TO postgres;

--
-- Name: order_detail_57; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_57 PARTITION OF public.order_detail
FOR VALUES IN ('57');


ALTER TABLE public.order_detail_57 OWNER TO postgres;

--
-- Name: order_detail_58; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_58 PARTITION OF public.order_detail
FOR VALUES IN ('58');


ALTER TABLE public.order_detail_58 OWNER TO postgres;

--
-- Name: order_detail_59; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_59 PARTITION OF public.order_detail
FOR VALUES IN ('59');


ALTER TABLE public.order_detail_59 OWNER TO postgres;

--
-- Name: order_detail_6; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_6 PARTITION OF public.order_detail
FOR VALUES IN ('6');


ALTER TABLE public.order_detail_6 OWNER TO postgres;

--
-- Name: order_detail_60; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_60 PARTITION OF public.order_detail
FOR VALUES IN ('60');


ALTER TABLE public.order_detail_60 OWNER TO postgres;

--
-- Name: order_detail_61; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_61 PARTITION OF public.order_detail
FOR VALUES IN ('61');


ALTER TABLE public.order_detail_61 OWNER TO postgres;

--
-- Name: order_detail_62; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_62 PARTITION OF public.order_detail
FOR VALUES IN ('62');


ALTER TABLE public.order_detail_62 OWNER TO postgres;

--
-- Name: order_detail_63; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_63 PARTITION OF public.order_detail
FOR VALUES IN ('63');


ALTER TABLE public.order_detail_63 OWNER TO postgres;

--
-- Name: order_detail_64; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_64 PARTITION OF public.order_detail
FOR VALUES IN ('64');


ALTER TABLE public.order_detail_64 OWNER TO postgres;

--
-- Name: order_detail_65; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_65 PARTITION OF public.order_detail
FOR VALUES IN ('65');


ALTER TABLE public.order_detail_65 OWNER TO postgres;

--
-- Name: order_detail_66; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_66 PARTITION OF public.order_detail
FOR VALUES IN ('66');


ALTER TABLE public.order_detail_66 OWNER TO postgres;

--
-- Name: order_detail_67; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_67 PARTITION OF public.order_detail
FOR VALUES IN ('67');


ALTER TABLE public.order_detail_67 OWNER TO postgres;

--
-- Name: order_detail_68; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_68 PARTITION OF public.order_detail
FOR VALUES IN ('68');


ALTER TABLE public.order_detail_68 OWNER TO postgres;

--
-- Name: order_detail_69; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_69 PARTITION OF public.order_detail
FOR VALUES IN ('69');


ALTER TABLE public.order_detail_69 OWNER TO postgres;

--
-- Name: order_detail_7; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_7 PARTITION OF public.order_detail
FOR VALUES IN ('7');


ALTER TABLE public.order_detail_7 OWNER TO postgres;

--
-- Name: order_detail_70; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_70 PARTITION OF public.order_detail
FOR VALUES IN ('70');


ALTER TABLE public.order_detail_70 OWNER TO postgres;

--
-- Name: order_detail_71; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_71 PARTITION OF public.order_detail
FOR VALUES IN ('71');


ALTER TABLE public.order_detail_71 OWNER TO postgres;

--
-- Name: order_detail_72; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_72 PARTITION OF public.order_detail
FOR VALUES IN ('72');


ALTER TABLE public.order_detail_72 OWNER TO postgres;

--
-- Name: order_detail_73; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_73 PARTITION OF public.order_detail
FOR VALUES IN ('73');


ALTER TABLE public.order_detail_73 OWNER TO postgres;

--
-- Name: order_detail_74; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_74 PARTITION OF public.order_detail
FOR VALUES IN ('74');


ALTER TABLE public.order_detail_74 OWNER TO postgres;

--
-- Name: order_detail_75; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_75 PARTITION OF public.order_detail
FOR VALUES IN ('75');


ALTER TABLE public.order_detail_75 OWNER TO postgres;

--
-- Name: order_detail_76; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_76 PARTITION OF public.order_detail
FOR VALUES IN ('76');


ALTER TABLE public.order_detail_76 OWNER TO postgres;

--
-- Name: order_detail_77; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_77 PARTITION OF public.order_detail
FOR VALUES IN ('77');


ALTER TABLE public.order_detail_77 OWNER TO postgres;

--
-- Name: order_detail_78; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_78 PARTITION OF public.order_detail
FOR VALUES IN ('78');


ALTER TABLE public.order_detail_78 OWNER TO postgres;

--
-- Name: order_detail_79; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_79 PARTITION OF public.order_detail
FOR VALUES IN ('79');


ALTER TABLE public.order_detail_79 OWNER TO postgres;

--
-- Name: order_detail_8; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_8 PARTITION OF public.order_detail
FOR VALUES IN ('8');


ALTER TABLE public.order_detail_8 OWNER TO postgres;

--
-- Name: order_detail_80; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_80 PARTITION OF public.order_detail
FOR VALUES IN ('80');


ALTER TABLE public.order_detail_80 OWNER TO postgres;

--
-- Name: order_detail_81; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_81 PARTITION OF public.order_detail
FOR VALUES IN ('81');


ALTER TABLE public.order_detail_81 OWNER TO postgres;

--
-- Name: order_detail_82; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_82 PARTITION OF public.order_detail
FOR VALUES IN ('82');


ALTER TABLE public.order_detail_82 OWNER TO postgres;

--
-- Name: order_detail_83; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_83 PARTITION OF public.order_detail
FOR VALUES IN ('83');


ALTER TABLE public.order_detail_83 OWNER TO postgres;

--
-- Name: order_detail_84; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_84 PARTITION OF public.order_detail
FOR VALUES IN ('84');


ALTER TABLE public.order_detail_84 OWNER TO postgres;

--
-- Name: order_detail_85; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_85 PARTITION OF public.order_detail
FOR VALUES IN ('85');


ALTER TABLE public.order_detail_85 OWNER TO postgres;

--
-- Name: order_detail_86; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_86 PARTITION OF public.order_detail
FOR VALUES IN ('86');


ALTER TABLE public.order_detail_86 OWNER TO postgres;

--
-- Name: order_detail_87; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_87 PARTITION OF public.order_detail
FOR VALUES IN ('87');


ALTER TABLE public.order_detail_87 OWNER TO postgres;

--
-- Name: order_detail_88; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_88 PARTITION OF public.order_detail
FOR VALUES IN ('88');


ALTER TABLE public.order_detail_88 OWNER TO postgres;

--
-- Name: order_detail_89; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_89 PARTITION OF public.order_detail
FOR VALUES IN ('89');


ALTER TABLE public.order_detail_89 OWNER TO postgres;

--
-- Name: order_detail_9; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_9 PARTITION OF public.order_detail
FOR VALUES IN ('9');


ALTER TABLE public.order_detail_9 OWNER TO postgres;

--
-- Name: order_detail_90; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_90 PARTITION OF public.order_detail
FOR VALUES IN ('90');


ALTER TABLE public.order_detail_90 OWNER TO postgres;

--
-- Name: order_detail_91; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_91 PARTITION OF public.order_detail
FOR VALUES IN ('91');


ALTER TABLE public.order_detail_91 OWNER TO postgres;

--
-- Name: order_detail_92; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_92 PARTITION OF public.order_detail
FOR VALUES IN ('92');


ALTER TABLE public.order_detail_92 OWNER TO postgres;

--
-- Name: order_detail_93; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_93 PARTITION OF public.order_detail
FOR VALUES IN ('93');


ALTER TABLE public.order_detail_93 OWNER TO postgres;

--
-- Name: order_detail_94; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_94 PARTITION OF public.order_detail
FOR VALUES IN ('94');


ALTER TABLE public.order_detail_94 OWNER TO postgres;

--
-- Name: order_detail_95; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_95 PARTITION OF public.order_detail
FOR VALUES IN ('95');


ALTER TABLE public.order_detail_95 OWNER TO postgres;

--
-- Name: order_detail_96; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_96 PARTITION OF public.order_detail
FOR VALUES IN ('96');


ALTER TABLE public.order_detail_96 OWNER TO postgres;

--
-- Name: order_detail_97; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_97 PARTITION OF public.order_detail
FOR VALUES IN ('97');


ALTER TABLE public.order_detail_97 OWNER TO postgres;

--
-- Name: order_detail_98; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_98 PARTITION OF public.order_detail
FOR VALUES IN ('98');


ALTER TABLE public.order_detail_98 OWNER TO postgres;

--
-- Name: order_detail_99; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_detail_99 PARTITION OF public.order_detail
FOR VALUES IN ('99');


ALTER TABLE public.order_detail_99 OWNER TO postgres;

--
-- Name: order_history; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history (
    id bigint NOT NULL,
    create_time timestamp with time zone DEFAULT now() NOT NULL,
    finish_time timestamp with time zone DEFAULT now() NOT NULL,
    user_id bigint NOT NULL,
    market character varying NOT NULL,
    source character varying NOT NULL,
    order_type integer NOT NULL,
    side integer NOT NULL,
    price numeric(30,8) NOT NULL,
    amount numeric(30,8) NOT NULL,
    taker_fee numeric(30,8) NOT NULL,
    maker_fee numeric(30,8) NOT NULL,
    deal_stock numeric(30,8) NOT NULL,
    deal_money numeric(30,16) NOT NULL,
    deal_fee numeric(30,16) NOT NULL
)
PARTITION BY LIST (((user_id % (100)::bigint)));


ALTER TABLE public.order_history OWNER TO postgres;

--
-- Name: order_history_0; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_0 PARTITION OF public.order_history
FOR VALUES IN ('0');


ALTER TABLE public.order_history_0 OWNER TO postgres;

--
-- Name: order_history_1; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_1 PARTITION OF public.order_history
FOR VALUES IN ('1');


ALTER TABLE public.order_history_1 OWNER TO postgres;

--
-- Name: order_history_10; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_10 PARTITION OF public.order_history
FOR VALUES IN ('10');


ALTER TABLE public.order_history_10 OWNER TO postgres;

--
-- Name: order_history_11; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_11 PARTITION OF public.order_history
FOR VALUES IN ('11');


ALTER TABLE public.order_history_11 OWNER TO postgres;

--
-- Name: order_history_12; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_12 PARTITION OF public.order_history
FOR VALUES IN ('12');


ALTER TABLE public.order_history_12 OWNER TO postgres;

--
-- Name: order_history_13; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_13 PARTITION OF public.order_history
FOR VALUES IN ('13');


ALTER TABLE public.order_history_13 OWNER TO postgres;

--
-- Name: order_history_14; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_14 PARTITION OF public.order_history
FOR VALUES IN ('14');


ALTER TABLE public.order_history_14 OWNER TO postgres;

--
-- Name: order_history_15; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_15 PARTITION OF public.order_history
FOR VALUES IN ('15');


ALTER TABLE public.order_history_15 OWNER TO postgres;

--
-- Name: order_history_16; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_16 PARTITION OF public.order_history
FOR VALUES IN ('16');


ALTER TABLE public.order_history_16 OWNER TO postgres;

--
-- Name: order_history_17; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_17 PARTITION OF public.order_history
FOR VALUES IN ('17');


ALTER TABLE public.order_history_17 OWNER TO postgres;

--
-- Name: order_history_18; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_18 PARTITION OF public.order_history
FOR VALUES IN ('18');


ALTER TABLE public.order_history_18 OWNER TO postgres;

--
-- Name: order_history_19; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_19 PARTITION OF public.order_history
FOR VALUES IN ('19');


ALTER TABLE public.order_history_19 OWNER TO postgres;

--
-- Name: order_history_2; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_2 PARTITION OF public.order_history
FOR VALUES IN ('2');


ALTER TABLE public.order_history_2 OWNER TO postgres;

--
-- Name: order_history_20; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_20 PARTITION OF public.order_history
FOR VALUES IN ('20');


ALTER TABLE public.order_history_20 OWNER TO postgres;

--
-- Name: order_history_21; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_21 PARTITION OF public.order_history
FOR VALUES IN ('21');


ALTER TABLE public.order_history_21 OWNER TO postgres;

--
-- Name: order_history_22; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_22 PARTITION OF public.order_history
FOR VALUES IN ('22');


ALTER TABLE public.order_history_22 OWNER TO postgres;

--
-- Name: order_history_23; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_23 PARTITION OF public.order_history
FOR VALUES IN ('23');


ALTER TABLE public.order_history_23 OWNER TO postgres;

--
-- Name: order_history_24; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_24 PARTITION OF public.order_history
FOR VALUES IN ('24');


ALTER TABLE public.order_history_24 OWNER TO postgres;

--
-- Name: order_history_25; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_25 PARTITION OF public.order_history
FOR VALUES IN ('25');


ALTER TABLE public.order_history_25 OWNER TO postgres;

--
-- Name: order_history_26; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_26 PARTITION OF public.order_history
FOR VALUES IN ('26');


ALTER TABLE public.order_history_26 OWNER TO postgres;

--
-- Name: order_history_27; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_27 PARTITION OF public.order_history
FOR VALUES IN ('27');


ALTER TABLE public.order_history_27 OWNER TO postgres;

--
-- Name: order_history_28; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_28 PARTITION OF public.order_history
FOR VALUES IN ('28');


ALTER TABLE public.order_history_28 OWNER TO postgres;

--
-- Name: order_history_29; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_29 PARTITION OF public.order_history
FOR VALUES IN ('29');


ALTER TABLE public.order_history_29 OWNER TO postgres;

--
-- Name: order_history_3; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_3 PARTITION OF public.order_history
FOR VALUES IN ('3');


ALTER TABLE public.order_history_3 OWNER TO postgres;

--
-- Name: order_history_30; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_30 PARTITION OF public.order_history
FOR VALUES IN ('30');


ALTER TABLE public.order_history_30 OWNER TO postgres;

--
-- Name: order_history_31; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_31 PARTITION OF public.order_history
FOR VALUES IN ('31');


ALTER TABLE public.order_history_31 OWNER TO postgres;

--
-- Name: order_history_32; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_32 PARTITION OF public.order_history
FOR VALUES IN ('32');


ALTER TABLE public.order_history_32 OWNER TO postgres;

--
-- Name: order_history_33; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_33 PARTITION OF public.order_history
FOR VALUES IN ('33');


ALTER TABLE public.order_history_33 OWNER TO postgres;

--
-- Name: order_history_34; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_34 PARTITION OF public.order_history
FOR VALUES IN ('34');


ALTER TABLE public.order_history_34 OWNER TO postgres;

--
-- Name: order_history_35; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_35 PARTITION OF public.order_history
FOR VALUES IN ('35');


ALTER TABLE public.order_history_35 OWNER TO postgres;

--
-- Name: order_history_36; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_36 PARTITION OF public.order_history
FOR VALUES IN ('36');


ALTER TABLE public.order_history_36 OWNER TO postgres;

--
-- Name: order_history_37; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_37 PARTITION OF public.order_history
FOR VALUES IN ('37');


ALTER TABLE public.order_history_37 OWNER TO postgres;

--
-- Name: order_history_38; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_38 PARTITION OF public.order_history
FOR VALUES IN ('38');


ALTER TABLE public.order_history_38 OWNER TO postgres;

--
-- Name: order_history_39; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_39 PARTITION OF public.order_history
FOR VALUES IN ('39');


ALTER TABLE public.order_history_39 OWNER TO postgres;

--
-- Name: order_history_4; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_4 PARTITION OF public.order_history
FOR VALUES IN ('4');


ALTER TABLE public.order_history_4 OWNER TO postgres;

--
-- Name: order_history_40; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_40 PARTITION OF public.order_history
FOR VALUES IN ('40');


ALTER TABLE public.order_history_40 OWNER TO postgres;

--
-- Name: order_history_41; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_41 PARTITION OF public.order_history
FOR VALUES IN ('41');


ALTER TABLE public.order_history_41 OWNER TO postgres;

--
-- Name: order_history_42; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_42 PARTITION OF public.order_history
FOR VALUES IN ('42');


ALTER TABLE public.order_history_42 OWNER TO postgres;

--
-- Name: order_history_43; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_43 PARTITION OF public.order_history
FOR VALUES IN ('43');


ALTER TABLE public.order_history_43 OWNER TO postgres;

--
-- Name: order_history_44; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_44 PARTITION OF public.order_history
FOR VALUES IN ('44');


ALTER TABLE public.order_history_44 OWNER TO postgres;

--
-- Name: order_history_45; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_45 PARTITION OF public.order_history
FOR VALUES IN ('45');


ALTER TABLE public.order_history_45 OWNER TO postgres;

--
-- Name: order_history_46; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_46 PARTITION OF public.order_history
FOR VALUES IN ('46');


ALTER TABLE public.order_history_46 OWNER TO postgres;

--
-- Name: order_history_47; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_47 PARTITION OF public.order_history
FOR VALUES IN ('47');


ALTER TABLE public.order_history_47 OWNER TO postgres;

--
-- Name: order_history_48; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_48 PARTITION OF public.order_history
FOR VALUES IN ('48');


ALTER TABLE public.order_history_48 OWNER TO postgres;

--
-- Name: order_history_49; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_49 PARTITION OF public.order_history
FOR VALUES IN ('49');


ALTER TABLE public.order_history_49 OWNER TO postgres;

--
-- Name: order_history_5; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_5 PARTITION OF public.order_history
FOR VALUES IN ('5');


ALTER TABLE public.order_history_5 OWNER TO postgres;

--
-- Name: order_history_50; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_50 PARTITION OF public.order_history
FOR VALUES IN ('50');


ALTER TABLE public.order_history_50 OWNER TO postgres;

--
-- Name: order_history_51; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_51 PARTITION OF public.order_history
FOR VALUES IN ('51');


ALTER TABLE public.order_history_51 OWNER TO postgres;

--
-- Name: order_history_52; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_52 PARTITION OF public.order_history
FOR VALUES IN ('52');


ALTER TABLE public.order_history_52 OWNER TO postgres;

--
-- Name: order_history_53; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_53 PARTITION OF public.order_history
FOR VALUES IN ('53');


ALTER TABLE public.order_history_53 OWNER TO postgres;

--
-- Name: order_history_54; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_54 PARTITION OF public.order_history
FOR VALUES IN ('54');


ALTER TABLE public.order_history_54 OWNER TO postgres;

--
-- Name: order_history_55; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_55 PARTITION OF public.order_history
FOR VALUES IN ('55');


ALTER TABLE public.order_history_55 OWNER TO postgres;

--
-- Name: order_history_56; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_56 PARTITION OF public.order_history
FOR VALUES IN ('56');


ALTER TABLE public.order_history_56 OWNER TO postgres;

--
-- Name: order_history_57; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_57 PARTITION OF public.order_history
FOR VALUES IN ('57');


ALTER TABLE public.order_history_57 OWNER TO postgres;

--
-- Name: order_history_58; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_58 PARTITION OF public.order_history
FOR VALUES IN ('58');


ALTER TABLE public.order_history_58 OWNER TO postgres;

--
-- Name: order_history_59; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_59 PARTITION OF public.order_history
FOR VALUES IN ('59');


ALTER TABLE public.order_history_59 OWNER TO postgres;

--
-- Name: order_history_6; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_6 PARTITION OF public.order_history
FOR VALUES IN ('6');


ALTER TABLE public.order_history_6 OWNER TO postgres;

--
-- Name: order_history_60; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_60 PARTITION OF public.order_history
FOR VALUES IN ('60');


ALTER TABLE public.order_history_60 OWNER TO postgres;

--
-- Name: order_history_61; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_61 PARTITION OF public.order_history
FOR VALUES IN ('61');


ALTER TABLE public.order_history_61 OWNER TO postgres;

--
-- Name: order_history_62; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_62 PARTITION OF public.order_history
FOR VALUES IN ('62');


ALTER TABLE public.order_history_62 OWNER TO postgres;

--
-- Name: order_history_63; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_63 PARTITION OF public.order_history
FOR VALUES IN ('63');


ALTER TABLE public.order_history_63 OWNER TO postgres;

--
-- Name: order_history_64; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_64 PARTITION OF public.order_history
FOR VALUES IN ('64');


ALTER TABLE public.order_history_64 OWNER TO postgres;

--
-- Name: order_history_65; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_65 PARTITION OF public.order_history
FOR VALUES IN ('65');


ALTER TABLE public.order_history_65 OWNER TO postgres;

--
-- Name: order_history_66; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_66 PARTITION OF public.order_history
FOR VALUES IN ('66');


ALTER TABLE public.order_history_66 OWNER TO postgres;

--
-- Name: order_history_67; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_67 PARTITION OF public.order_history
FOR VALUES IN ('67');


ALTER TABLE public.order_history_67 OWNER TO postgres;

--
-- Name: order_history_68; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_68 PARTITION OF public.order_history
FOR VALUES IN ('68');


ALTER TABLE public.order_history_68 OWNER TO postgres;

--
-- Name: order_history_69; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_69 PARTITION OF public.order_history
FOR VALUES IN ('69');


ALTER TABLE public.order_history_69 OWNER TO postgres;

--
-- Name: order_history_7; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_7 PARTITION OF public.order_history
FOR VALUES IN ('7');


ALTER TABLE public.order_history_7 OWNER TO postgres;

--
-- Name: order_history_70; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_70 PARTITION OF public.order_history
FOR VALUES IN ('70');


ALTER TABLE public.order_history_70 OWNER TO postgres;

--
-- Name: order_history_71; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_71 PARTITION OF public.order_history
FOR VALUES IN ('71');


ALTER TABLE public.order_history_71 OWNER TO postgres;

--
-- Name: order_history_72; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_72 PARTITION OF public.order_history
FOR VALUES IN ('72');


ALTER TABLE public.order_history_72 OWNER TO postgres;

--
-- Name: order_history_73; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_73 PARTITION OF public.order_history
FOR VALUES IN ('73');


ALTER TABLE public.order_history_73 OWNER TO postgres;

--
-- Name: order_history_74; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_74 PARTITION OF public.order_history
FOR VALUES IN ('74');


ALTER TABLE public.order_history_74 OWNER TO postgres;

--
-- Name: order_history_75; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_75 PARTITION OF public.order_history
FOR VALUES IN ('75');


ALTER TABLE public.order_history_75 OWNER TO postgres;

--
-- Name: order_history_76; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_76 PARTITION OF public.order_history
FOR VALUES IN ('76');


ALTER TABLE public.order_history_76 OWNER TO postgres;

--
-- Name: order_history_77; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_77 PARTITION OF public.order_history
FOR VALUES IN ('77');


ALTER TABLE public.order_history_77 OWNER TO postgres;

--
-- Name: order_history_78; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_78 PARTITION OF public.order_history
FOR VALUES IN ('78');


ALTER TABLE public.order_history_78 OWNER TO postgres;

--
-- Name: order_history_79; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_79 PARTITION OF public.order_history
FOR VALUES IN ('79');


ALTER TABLE public.order_history_79 OWNER TO postgres;

--
-- Name: order_history_8; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_8 PARTITION OF public.order_history
FOR VALUES IN ('8');


ALTER TABLE public.order_history_8 OWNER TO postgres;

--
-- Name: order_history_80; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_80 PARTITION OF public.order_history
FOR VALUES IN ('80');


ALTER TABLE public.order_history_80 OWNER TO postgres;

--
-- Name: order_history_81; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_81 PARTITION OF public.order_history
FOR VALUES IN ('81');


ALTER TABLE public.order_history_81 OWNER TO postgres;

--
-- Name: order_history_82; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_82 PARTITION OF public.order_history
FOR VALUES IN ('82');


ALTER TABLE public.order_history_82 OWNER TO postgres;

--
-- Name: order_history_83; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_83 PARTITION OF public.order_history
FOR VALUES IN ('83');


ALTER TABLE public.order_history_83 OWNER TO postgres;

--
-- Name: order_history_84; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_84 PARTITION OF public.order_history
FOR VALUES IN ('84');


ALTER TABLE public.order_history_84 OWNER TO postgres;

--
-- Name: order_history_85; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_85 PARTITION OF public.order_history
FOR VALUES IN ('85');


ALTER TABLE public.order_history_85 OWNER TO postgres;

--
-- Name: order_history_86; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_86 PARTITION OF public.order_history
FOR VALUES IN ('86');


ALTER TABLE public.order_history_86 OWNER TO postgres;

--
-- Name: order_history_87; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_87 PARTITION OF public.order_history
FOR VALUES IN ('87');


ALTER TABLE public.order_history_87 OWNER TO postgres;

--
-- Name: order_history_88; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_88 PARTITION OF public.order_history
FOR VALUES IN ('88');


ALTER TABLE public.order_history_88 OWNER TO postgres;

--
-- Name: order_history_89; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_89 PARTITION OF public.order_history
FOR VALUES IN ('89');


ALTER TABLE public.order_history_89 OWNER TO postgres;

--
-- Name: order_history_9; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_9 PARTITION OF public.order_history
FOR VALUES IN ('9');


ALTER TABLE public.order_history_9 OWNER TO postgres;

--
-- Name: order_history_90; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_90 PARTITION OF public.order_history
FOR VALUES IN ('90');


ALTER TABLE public.order_history_90 OWNER TO postgres;

--
-- Name: order_history_91; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_91 PARTITION OF public.order_history
FOR VALUES IN ('91');


ALTER TABLE public.order_history_91 OWNER TO postgres;

--
-- Name: order_history_92; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_92 PARTITION OF public.order_history
FOR VALUES IN ('92');


ALTER TABLE public.order_history_92 OWNER TO postgres;

--
-- Name: order_history_93; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_93 PARTITION OF public.order_history
FOR VALUES IN ('93');


ALTER TABLE public.order_history_93 OWNER TO postgres;

--
-- Name: order_history_94; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_94 PARTITION OF public.order_history
FOR VALUES IN ('94');


ALTER TABLE public.order_history_94 OWNER TO postgres;

--
-- Name: order_history_95; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_95 PARTITION OF public.order_history
FOR VALUES IN ('95');


ALTER TABLE public.order_history_95 OWNER TO postgres;

--
-- Name: order_history_96; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_96 PARTITION OF public.order_history
FOR VALUES IN ('96');


ALTER TABLE public.order_history_96 OWNER TO postgres;

--
-- Name: order_history_97; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_97 PARTITION OF public.order_history
FOR VALUES IN ('97');


ALTER TABLE public.order_history_97 OWNER TO postgres;

--
-- Name: order_history_98; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_98 PARTITION OF public.order_history
FOR VALUES IN ('98');


ALTER TABLE public.order_history_98 OWNER TO postgres;

--
-- Name: order_history_99; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.order_history_99 PARTITION OF public.order_history
FOR VALUES IN ('99');


ALTER TABLE public.order_history_99 OWNER TO postgres;

--
-- Name: user_deal_history; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history (
    id bigint NOT NULL,
    "time" timestamp with time zone DEFAULT now() NOT NULL,
    user_id bigint NOT NULL,
    market character varying NOT NULL,
    deal_id bigint NOT NULL,
    order_id bigint NOT NULL,
    deal_order_id bigint NOT NULL,
    side integer NOT NULL,
    role integer NOT NULL,
    price numeric(30,8) NOT NULL,
    amount numeric(30,8) NOT NULL,
    deal numeric(30,16) NOT NULL,
    fee numeric(30,16) NOT NULL,
    deal_fee numeric(30,16) NOT NULL
)
PARTITION BY LIST (((user_id % (100)::bigint)));


ALTER TABLE public.user_deal_history OWNER TO postgres;

--
-- Name: user_deal_history_0; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_0 PARTITION OF public.user_deal_history
FOR VALUES IN ('0');


ALTER TABLE public.user_deal_history_0 OWNER TO postgres;

--
-- Name: user_deal_history_1; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_1 PARTITION OF public.user_deal_history
FOR VALUES IN ('1');


ALTER TABLE public.user_deal_history_1 OWNER TO postgres;

--
-- Name: user_deal_history_10; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_10 PARTITION OF public.user_deal_history
FOR VALUES IN ('10');


ALTER TABLE public.user_deal_history_10 OWNER TO postgres;

--
-- Name: user_deal_history_11; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_11 PARTITION OF public.user_deal_history
FOR VALUES IN ('11');


ALTER TABLE public.user_deal_history_11 OWNER TO postgres;

--
-- Name: user_deal_history_12; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_12 PARTITION OF public.user_deal_history
FOR VALUES IN ('12');


ALTER TABLE public.user_deal_history_12 OWNER TO postgres;

--
-- Name: user_deal_history_13; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_13 PARTITION OF public.user_deal_history
FOR VALUES IN ('13');


ALTER TABLE public.user_deal_history_13 OWNER TO postgres;

--
-- Name: user_deal_history_14; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_14 PARTITION OF public.user_deal_history
FOR VALUES IN ('14');


ALTER TABLE public.user_deal_history_14 OWNER TO postgres;

--
-- Name: user_deal_history_15; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_15 PARTITION OF public.user_deal_history
FOR VALUES IN ('15');


ALTER TABLE public.user_deal_history_15 OWNER TO postgres;

--
-- Name: user_deal_history_16; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_16 PARTITION OF public.user_deal_history
FOR VALUES IN ('16');


ALTER TABLE public.user_deal_history_16 OWNER TO postgres;

--
-- Name: user_deal_history_17; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_17 PARTITION OF public.user_deal_history
FOR VALUES IN ('17');


ALTER TABLE public.user_deal_history_17 OWNER TO postgres;

--
-- Name: user_deal_history_18; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_18 PARTITION OF public.user_deal_history
FOR VALUES IN ('18');


ALTER TABLE public.user_deal_history_18 OWNER TO postgres;

--
-- Name: user_deal_history_19; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_19 PARTITION OF public.user_deal_history
FOR VALUES IN ('19');


ALTER TABLE public.user_deal_history_19 OWNER TO postgres;

--
-- Name: user_deal_history_2; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_2 PARTITION OF public.user_deal_history
FOR VALUES IN ('2');


ALTER TABLE public.user_deal_history_2 OWNER TO postgres;

--
-- Name: user_deal_history_20; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_20 PARTITION OF public.user_deal_history
FOR VALUES IN ('20');


ALTER TABLE public.user_deal_history_20 OWNER TO postgres;

--
-- Name: user_deal_history_21; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_21 PARTITION OF public.user_deal_history
FOR VALUES IN ('21');


ALTER TABLE public.user_deal_history_21 OWNER TO postgres;

--
-- Name: user_deal_history_22; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_22 PARTITION OF public.user_deal_history
FOR VALUES IN ('22');


ALTER TABLE public.user_deal_history_22 OWNER TO postgres;

--
-- Name: user_deal_history_23; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_23 PARTITION OF public.user_deal_history
FOR VALUES IN ('23');


ALTER TABLE public.user_deal_history_23 OWNER TO postgres;

--
-- Name: user_deal_history_24; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_24 PARTITION OF public.user_deal_history
FOR VALUES IN ('24');


ALTER TABLE public.user_deal_history_24 OWNER TO postgres;

--
-- Name: user_deal_history_25; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_25 PARTITION OF public.user_deal_history
FOR VALUES IN ('25');


ALTER TABLE public.user_deal_history_25 OWNER TO postgres;

--
-- Name: user_deal_history_26; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_26 PARTITION OF public.user_deal_history
FOR VALUES IN ('26');


ALTER TABLE public.user_deal_history_26 OWNER TO postgres;

--
-- Name: user_deal_history_27; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_27 PARTITION OF public.user_deal_history
FOR VALUES IN ('27');


ALTER TABLE public.user_deal_history_27 OWNER TO postgres;

--
-- Name: user_deal_history_28; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_28 PARTITION OF public.user_deal_history
FOR VALUES IN ('28');


ALTER TABLE public.user_deal_history_28 OWNER TO postgres;

--
-- Name: user_deal_history_29; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_29 PARTITION OF public.user_deal_history
FOR VALUES IN ('29');


ALTER TABLE public.user_deal_history_29 OWNER TO postgres;

--
-- Name: user_deal_history_3; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_3 PARTITION OF public.user_deal_history
FOR VALUES IN ('3');


ALTER TABLE public.user_deal_history_3 OWNER TO postgres;

--
-- Name: user_deal_history_30; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_30 PARTITION OF public.user_deal_history
FOR VALUES IN ('30');


ALTER TABLE public.user_deal_history_30 OWNER TO postgres;

--
-- Name: user_deal_history_31; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_31 PARTITION OF public.user_deal_history
FOR VALUES IN ('31');


ALTER TABLE public.user_deal_history_31 OWNER TO postgres;

--
-- Name: user_deal_history_32; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_32 PARTITION OF public.user_deal_history
FOR VALUES IN ('32');


ALTER TABLE public.user_deal_history_32 OWNER TO postgres;

--
-- Name: user_deal_history_33; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_33 PARTITION OF public.user_deal_history
FOR VALUES IN ('33');


ALTER TABLE public.user_deal_history_33 OWNER TO postgres;

--
-- Name: user_deal_history_34; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_34 PARTITION OF public.user_deal_history
FOR VALUES IN ('34');


ALTER TABLE public.user_deal_history_34 OWNER TO postgres;

--
-- Name: user_deal_history_35; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_35 PARTITION OF public.user_deal_history
FOR VALUES IN ('35');


ALTER TABLE public.user_deal_history_35 OWNER TO postgres;

--
-- Name: user_deal_history_36; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_36 PARTITION OF public.user_deal_history
FOR VALUES IN ('36');


ALTER TABLE public.user_deal_history_36 OWNER TO postgres;

--
-- Name: user_deal_history_37; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_37 PARTITION OF public.user_deal_history
FOR VALUES IN ('37');


ALTER TABLE public.user_deal_history_37 OWNER TO postgres;

--
-- Name: user_deal_history_38; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_38 PARTITION OF public.user_deal_history
FOR VALUES IN ('38');


ALTER TABLE public.user_deal_history_38 OWNER TO postgres;

--
-- Name: user_deal_history_39; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_39 PARTITION OF public.user_deal_history
FOR VALUES IN ('39');


ALTER TABLE public.user_deal_history_39 OWNER TO postgres;

--
-- Name: user_deal_history_4; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_4 PARTITION OF public.user_deal_history
FOR VALUES IN ('4');


ALTER TABLE public.user_deal_history_4 OWNER TO postgres;

--
-- Name: user_deal_history_40; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_40 PARTITION OF public.user_deal_history
FOR VALUES IN ('40');


ALTER TABLE public.user_deal_history_40 OWNER TO postgres;

--
-- Name: user_deal_history_41; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_41 PARTITION OF public.user_deal_history
FOR VALUES IN ('41');


ALTER TABLE public.user_deal_history_41 OWNER TO postgres;

--
-- Name: user_deal_history_42; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_42 PARTITION OF public.user_deal_history
FOR VALUES IN ('42');


ALTER TABLE public.user_deal_history_42 OWNER TO postgres;

--
-- Name: user_deal_history_43; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_43 PARTITION OF public.user_deal_history
FOR VALUES IN ('43');


ALTER TABLE public.user_deal_history_43 OWNER TO postgres;

--
-- Name: user_deal_history_44; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_44 PARTITION OF public.user_deal_history
FOR VALUES IN ('44');


ALTER TABLE public.user_deal_history_44 OWNER TO postgres;

--
-- Name: user_deal_history_45; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_45 PARTITION OF public.user_deal_history
FOR VALUES IN ('45');


ALTER TABLE public.user_deal_history_45 OWNER TO postgres;

--
-- Name: user_deal_history_46; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_46 PARTITION OF public.user_deal_history
FOR VALUES IN ('46');


ALTER TABLE public.user_deal_history_46 OWNER TO postgres;

--
-- Name: user_deal_history_47; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_47 PARTITION OF public.user_deal_history
FOR VALUES IN ('47');


ALTER TABLE public.user_deal_history_47 OWNER TO postgres;

--
-- Name: user_deal_history_48; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_48 PARTITION OF public.user_deal_history
FOR VALUES IN ('48');


ALTER TABLE public.user_deal_history_48 OWNER TO postgres;

--
-- Name: user_deal_history_49; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_49 PARTITION OF public.user_deal_history
FOR VALUES IN ('49');


ALTER TABLE public.user_deal_history_49 OWNER TO postgres;

--
-- Name: user_deal_history_5; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_5 PARTITION OF public.user_deal_history
FOR VALUES IN ('5');


ALTER TABLE public.user_deal_history_5 OWNER TO postgres;

--
-- Name: user_deal_history_50; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_50 PARTITION OF public.user_deal_history
FOR VALUES IN ('50');


ALTER TABLE public.user_deal_history_50 OWNER TO postgres;

--
-- Name: user_deal_history_51; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_51 PARTITION OF public.user_deal_history
FOR VALUES IN ('51');


ALTER TABLE public.user_deal_history_51 OWNER TO postgres;

--
-- Name: user_deal_history_52; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_52 PARTITION OF public.user_deal_history
FOR VALUES IN ('52');


ALTER TABLE public.user_deal_history_52 OWNER TO postgres;

--
-- Name: user_deal_history_53; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_53 PARTITION OF public.user_deal_history
FOR VALUES IN ('53');


ALTER TABLE public.user_deal_history_53 OWNER TO postgres;

--
-- Name: user_deal_history_54; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_54 PARTITION OF public.user_deal_history
FOR VALUES IN ('54');


ALTER TABLE public.user_deal_history_54 OWNER TO postgres;

--
-- Name: user_deal_history_55; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_55 PARTITION OF public.user_deal_history
FOR VALUES IN ('55');


ALTER TABLE public.user_deal_history_55 OWNER TO postgres;

--
-- Name: user_deal_history_56; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_56 PARTITION OF public.user_deal_history
FOR VALUES IN ('56');


ALTER TABLE public.user_deal_history_56 OWNER TO postgres;

--
-- Name: user_deal_history_57; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_57 PARTITION OF public.user_deal_history
FOR VALUES IN ('57');


ALTER TABLE public.user_deal_history_57 OWNER TO postgres;

--
-- Name: user_deal_history_58; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_58 PARTITION OF public.user_deal_history
FOR VALUES IN ('58');


ALTER TABLE public.user_deal_history_58 OWNER TO postgres;

--
-- Name: user_deal_history_59; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_59 PARTITION OF public.user_deal_history
FOR VALUES IN ('59');


ALTER TABLE public.user_deal_history_59 OWNER TO postgres;

--
-- Name: user_deal_history_6; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_6 PARTITION OF public.user_deal_history
FOR VALUES IN ('6');


ALTER TABLE public.user_deal_history_6 OWNER TO postgres;

--
-- Name: user_deal_history_60; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_60 PARTITION OF public.user_deal_history
FOR VALUES IN ('60');


ALTER TABLE public.user_deal_history_60 OWNER TO postgres;

--
-- Name: user_deal_history_61; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_61 PARTITION OF public.user_deal_history
FOR VALUES IN ('61');


ALTER TABLE public.user_deal_history_61 OWNER TO postgres;

--
-- Name: user_deal_history_62; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_62 PARTITION OF public.user_deal_history
FOR VALUES IN ('62');


ALTER TABLE public.user_deal_history_62 OWNER TO postgres;

--
-- Name: user_deal_history_63; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_63 PARTITION OF public.user_deal_history
FOR VALUES IN ('63');


ALTER TABLE public.user_deal_history_63 OWNER TO postgres;

--
-- Name: user_deal_history_64; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_64 PARTITION OF public.user_deal_history
FOR VALUES IN ('64');


ALTER TABLE public.user_deal_history_64 OWNER TO postgres;

--
-- Name: user_deal_history_65; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_65 PARTITION OF public.user_deal_history
FOR VALUES IN ('65');


ALTER TABLE public.user_deal_history_65 OWNER TO postgres;

--
-- Name: user_deal_history_66; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_66 PARTITION OF public.user_deal_history
FOR VALUES IN ('66');


ALTER TABLE public.user_deal_history_66 OWNER TO postgres;

--
-- Name: user_deal_history_67; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_67 PARTITION OF public.user_deal_history
FOR VALUES IN ('67');


ALTER TABLE public.user_deal_history_67 OWNER TO postgres;

--
-- Name: user_deal_history_68; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_68 PARTITION OF public.user_deal_history
FOR VALUES IN ('68');


ALTER TABLE public.user_deal_history_68 OWNER TO postgres;

--
-- Name: user_deal_history_69; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_69 PARTITION OF public.user_deal_history
FOR VALUES IN ('69');


ALTER TABLE public.user_deal_history_69 OWNER TO postgres;

--
-- Name: user_deal_history_7; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_7 PARTITION OF public.user_deal_history
FOR VALUES IN ('7');


ALTER TABLE public.user_deal_history_7 OWNER TO postgres;

--
-- Name: user_deal_history_70; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_70 PARTITION OF public.user_deal_history
FOR VALUES IN ('70');


ALTER TABLE public.user_deal_history_70 OWNER TO postgres;

--
-- Name: user_deal_history_71; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_71 PARTITION OF public.user_deal_history
FOR VALUES IN ('71');


ALTER TABLE public.user_deal_history_71 OWNER TO postgres;

--
-- Name: user_deal_history_72; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_72 PARTITION OF public.user_deal_history
FOR VALUES IN ('72');


ALTER TABLE public.user_deal_history_72 OWNER TO postgres;

--
-- Name: user_deal_history_73; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_73 PARTITION OF public.user_deal_history
FOR VALUES IN ('73');


ALTER TABLE public.user_deal_history_73 OWNER TO postgres;

--
-- Name: user_deal_history_74; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_74 PARTITION OF public.user_deal_history
FOR VALUES IN ('74');


ALTER TABLE public.user_deal_history_74 OWNER TO postgres;

--
-- Name: user_deal_history_75; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_75 PARTITION OF public.user_deal_history
FOR VALUES IN ('75');


ALTER TABLE public.user_deal_history_75 OWNER TO postgres;

--
-- Name: user_deal_history_76; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_76 PARTITION OF public.user_deal_history
FOR VALUES IN ('76');


ALTER TABLE public.user_deal_history_76 OWNER TO postgres;

--
-- Name: user_deal_history_77; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_77 PARTITION OF public.user_deal_history
FOR VALUES IN ('77');


ALTER TABLE public.user_deal_history_77 OWNER TO postgres;

--
-- Name: user_deal_history_78; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_78 PARTITION OF public.user_deal_history
FOR VALUES IN ('78');


ALTER TABLE public.user_deal_history_78 OWNER TO postgres;

--
-- Name: user_deal_history_79; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_79 PARTITION OF public.user_deal_history
FOR VALUES IN ('79');


ALTER TABLE public.user_deal_history_79 OWNER TO postgres;

--
-- Name: user_deal_history_8; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_8 PARTITION OF public.user_deal_history
FOR VALUES IN ('8');


ALTER TABLE public.user_deal_history_8 OWNER TO postgres;

--
-- Name: user_deal_history_80; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_80 PARTITION OF public.user_deal_history
FOR VALUES IN ('80');


ALTER TABLE public.user_deal_history_80 OWNER TO postgres;

--
-- Name: user_deal_history_81; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_81 PARTITION OF public.user_deal_history
FOR VALUES IN ('81');


ALTER TABLE public.user_deal_history_81 OWNER TO postgres;

--
-- Name: user_deal_history_82; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_82 PARTITION OF public.user_deal_history
FOR VALUES IN ('82');


ALTER TABLE public.user_deal_history_82 OWNER TO postgres;

--
-- Name: user_deal_history_83; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_83 PARTITION OF public.user_deal_history
FOR VALUES IN ('83');


ALTER TABLE public.user_deal_history_83 OWNER TO postgres;

--
-- Name: user_deal_history_84; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_84 PARTITION OF public.user_deal_history
FOR VALUES IN ('84');


ALTER TABLE public.user_deal_history_84 OWNER TO postgres;

--
-- Name: user_deal_history_85; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_85 PARTITION OF public.user_deal_history
FOR VALUES IN ('85');


ALTER TABLE public.user_deal_history_85 OWNER TO postgres;

--
-- Name: user_deal_history_86; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_86 PARTITION OF public.user_deal_history
FOR VALUES IN ('86');


ALTER TABLE public.user_deal_history_86 OWNER TO postgres;

--
-- Name: user_deal_history_87; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_87 PARTITION OF public.user_deal_history
FOR VALUES IN ('87');


ALTER TABLE public.user_deal_history_87 OWNER TO postgres;

--
-- Name: user_deal_history_88; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_88 PARTITION OF public.user_deal_history
FOR VALUES IN ('88');


ALTER TABLE public.user_deal_history_88 OWNER TO postgres;

--
-- Name: user_deal_history_89; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_89 PARTITION OF public.user_deal_history
FOR VALUES IN ('89');


ALTER TABLE public.user_deal_history_89 OWNER TO postgres;

--
-- Name: user_deal_history_9; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_9 PARTITION OF public.user_deal_history
FOR VALUES IN ('9');


ALTER TABLE public.user_deal_history_9 OWNER TO postgres;

--
-- Name: user_deal_history_90; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_90 PARTITION OF public.user_deal_history
FOR VALUES IN ('90');


ALTER TABLE public.user_deal_history_90 OWNER TO postgres;

--
-- Name: user_deal_history_91; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_91 PARTITION OF public.user_deal_history
FOR VALUES IN ('91');


ALTER TABLE public.user_deal_history_91 OWNER TO postgres;

--
-- Name: user_deal_history_92; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_92 PARTITION OF public.user_deal_history
FOR VALUES IN ('92');


ALTER TABLE public.user_deal_history_92 OWNER TO postgres;

--
-- Name: user_deal_history_93; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_93 PARTITION OF public.user_deal_history
FOR VALUES IN ('93');


ALTER TABLE public.user_deal_history_93 OWNER TO postgres;

--
-- Name: user_deal_history_94; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_94 PARTITION OF public.user_deal_history
FOR VALUES IN ('94');


ALTER TABLE public.user_deal_history_94 OWNER TO postgres;

--
-- Name: user_deal_history_95; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_95 PARTITION OF public.user_deal_history
FOR VALUES IN ('95');


ALTER TABLE public.user_deal_history_95 OWNER TO postgres;

--
-- Name: user_deal_history_96; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_96 PARTITION OF public.user_deal_history
FOR VALUES IN ('96');


ALTER TABLE public.user_deal_history_96 OWNER TO postgres;

--
-- Name: user_deal_history_97; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_97 PARTITION OF public.user_deal_history
FOR VALUES IN ('97');


ALTER TABLE public.user_deal_history_97 OWNER TO postgres;

--
-- Name: user_deal_history_98; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_98 PARTITION OF public.user_deal_history
FOR VALUES IN ('98');


ALTER TABLE public.user_deal_history_98 OWNER TO postgres;

--
-- Name: user_deal_history_99; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_deal_history_99 PARTITION OF public.user_deal_history
FOR VALUES IN ('99');


ALTER TABLE public.user_deal_history_99 OWNER TO postgres;

--
-- Name: user_deal_history_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.user_deal_history ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.user_deal_history_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: balance_history_0 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_0 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_1 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_1 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_10 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_10 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_11 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_11 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_12 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_12 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_13 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_13 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_14 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_14 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_15 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_15 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_16 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_16 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_17 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_17 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_18 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_18 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_19 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_19 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_2 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_2 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_20 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_20 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_21 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_21 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_22 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_22 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_23 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_23 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_24 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_24 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_25 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_25 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_26 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_26 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_27 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_27 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_28 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_28 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_29 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_29 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_3 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_3 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_30 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_30 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_31 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_31 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_32 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_32 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_33 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_33 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_34 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_34 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_35 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_35 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_36 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_36 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_37 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_37 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_38 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_38 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_39 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_39 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_4 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_4 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_40 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_40 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_41 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_41 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_42 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_42 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_43 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_43 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_44 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_44 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_45 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_45 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_46 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_46 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_47 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_47 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_48 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_48 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_49 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_49 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_5 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_5 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_50 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_50 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_51 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_51 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_52 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_52 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_53 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_53 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_54 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_54 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_55 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_55 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_56 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_56 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_57 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_57 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_58 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_58 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_59 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_59 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_6 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_6 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_60 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_60 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_61 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_61 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_62 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_62 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_63 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_63 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_64 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_64 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_65 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_65 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_66 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_66 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_67 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_67 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_68 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_68 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_69 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_69 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_7 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_7 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_70 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_70 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_71 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_71 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_72 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_72 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_73 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_73 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_74 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_74 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_75 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_75 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_76 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_76 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_77 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_77 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_78 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_78 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_79 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_79 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_8 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_8 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_80 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_80 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_81 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_81 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_82 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_82 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_83 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_83 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_84 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_84 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_85 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_85 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_86 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_86 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_87 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_87 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_88 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_88 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_89 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_89 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_9 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_9 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_90 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_90 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_91 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_91 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_92 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_92 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_93 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_93 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_94 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_94 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_95 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_95 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_96 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_96 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_97 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_97 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_98 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_98 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: balance_history_99 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.balance_history_99 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_0 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_0 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_1 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_1 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_10 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_10 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_11 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_11 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_12 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_12 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_13 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_13 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_14 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_14 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_15 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_15 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_16 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_16 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_17 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_17 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_18 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_18 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_19 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_19 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_2 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_2 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_20 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_20 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_21 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_21 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_22 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_22 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_23 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_23 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_24 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_24 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_25 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_25 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_26 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_26 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_27 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_27 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_28 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_28 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_29 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_29 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_3 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_3 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_30 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_30 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_31 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_31 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_32 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_32 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_33 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_33 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_34 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_34 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_35 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_35 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_36 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_36 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_37 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_37 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_38 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_38 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_39 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_39 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_4 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_4 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_40 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_40 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_41 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_41 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_42 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_42 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_43 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_43 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_44 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_44 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_45 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_45 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_46 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_46 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_47 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_47 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_48 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_48 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_49 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_49 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_5 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_5 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_50 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_50 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_51 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_51 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_52 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_52 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_53 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_53 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_54 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_54 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_55 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_55 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_56 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_56 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_57 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_57 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_58 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_58 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_59 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_59 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_6 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_6 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_60 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_60 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_61 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_61 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_62 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_62 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_63 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_63 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_64 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_64 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_65 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_65 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_66 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_66 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_67 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_67 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_68 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_68 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_69 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_69 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_7 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_7 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_70 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_70 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_71 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_71 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_72 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_72 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_73 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_73 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_74 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_74 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_75 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_75 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_76 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_76 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_77 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_77 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_78 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_78 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_79 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_79 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_8 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_8 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_80 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_80 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_81 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_81 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_82 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_82 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_83 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_83 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_84 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_84 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_85 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_85 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_86 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_86 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_87 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_87 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_88 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_88 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_89 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_89 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_9 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_9 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_90 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_90 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_91 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_91 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_92 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_92 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_93 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_93 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_94 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_94 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_95 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_95 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_96 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_96 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_97 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_97 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_98 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_98 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: deal_history_99 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.deal_history_99 ALTER COLUMN "time" SET DEFAULT now();



--
-- Name: order_detail_0 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_0 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_0 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_0 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_1 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_1 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_1 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_1 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_10 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_10 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_10 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_10 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_11 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_11 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_11 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_11 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_12 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_12 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_12 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_12 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_13 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_13 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_13 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_13 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_14 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_14 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_14 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_14 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_15 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_15 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_15 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_15 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_16 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_16 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_16 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_16 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_17 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_17 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_17 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_17 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_18 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_18 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_18 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_18 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_19 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_19 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_19 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_19 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_2 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_2 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_2 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_2 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_20 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_20 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_20 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_20 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_21 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_21 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_21 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_21 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_22 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_22 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_22 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_22 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_23 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_23 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_23 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_23 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_24 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_24 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_24 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_24 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_25 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_25 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_25 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_25 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_26 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_26 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_26 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_26 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_27 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_27 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_27 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_27 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_28 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_28 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_28 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_28 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_29 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_29 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_29 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_29 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_3 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_3 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_3 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_3 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_30 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_30 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_30 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_30 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_31 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_31 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_31 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_31 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_32 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_32 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_32 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_32 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_33 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_33 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_33 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_33 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_34 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_34 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_34 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_34 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_35 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_35 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_35 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_35 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_36 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_36 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_36 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_36 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_37 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_37 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_37 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_37 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_38 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_38 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_38 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_38 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_39 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_39 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_39 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_39 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_4 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_4 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_4 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_4 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_40 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_40 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_40 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_40 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_41 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_41 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_41 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_41 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_42 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_42 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_42 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_42 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_43 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_43 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_43 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_43 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_44 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_44 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_44 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_44 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_45 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_45 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_45 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_45 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_46 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_46 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_46 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_46 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_47 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_47 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_47 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_47 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_48 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_48 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_48 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_48 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_49 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_49 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_49 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_49 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_5 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_5 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_5 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_5 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_50 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_50 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_50 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_50 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_51 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_51 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_51 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_51 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_52 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_52 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_52 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_52 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_53 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_53 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_53 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_53 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_54 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_54 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_54 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_54 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_55 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_55 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_55 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_55 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_56 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_56 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_56 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_56 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_57 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_57 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_57 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_57 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_58 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_58 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_58 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_58 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_59 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_59 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_59 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_59 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_6 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_6 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_6 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_6 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_60 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_60 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_60 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_60 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_61 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_61 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_61 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_61 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_62 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_62 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_62 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_62 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_63 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_63 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_63 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_63 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_64 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_64 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_64 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_64 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_65 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_65 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_65 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_65 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_66 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_66 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_66 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_66 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_67 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_67 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_67 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_67 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_68 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_68 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_68 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_68 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_69 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_69 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_69 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_69 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_7 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_7 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_7 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_7 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_70 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_70 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_70 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_70 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_71 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_71 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_71 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_71 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_72 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_72 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_72 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_72 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_73 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_73 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_73 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_73 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_74 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_74 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_74 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_74 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_75 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_75 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_75 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_75 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_76 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_76 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_76 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_76 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_77 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_77 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_77 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_77 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_78 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_78 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_78 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_78 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_79 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_79 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_79 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_79 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_8 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_8 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_8 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_8 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_80 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_80 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_80 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_80 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_81 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_81 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_81 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_81 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_82 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_82 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_82 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_82 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_83 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_83 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_83 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_83 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_84 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_84 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_84 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_84 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_85 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_85 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_85 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_85 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_86 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_86 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_86 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_86 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_87 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_87 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_87 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_87 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_88 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_88 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_88 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_88 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_89 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_89 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_89 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_89 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_9 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_9 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_9 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_9 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_90 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_90 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_90 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_90 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_91 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_91 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_91 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_91 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_92 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_92 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_92 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_92 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_93 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_93 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_93 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_93 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_94 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_94 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_94 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_94 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_95 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_95 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_95 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_95 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_96 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_96 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_96 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_96 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_97 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_97 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_97 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_97 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_98 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_98 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_98 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_98 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_detail_99 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_99 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_detail_99 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_detail_99 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_0 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_0 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_0 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_0 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_1 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_1 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_1 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_1 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_10 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_10 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_10 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_10 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_11 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_11 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_11 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_11 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_12 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_12 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_12 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_12 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_13 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_13 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_13 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_13 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_14 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_14 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_14 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_14 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_15 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_15 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_15 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_15 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_16 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_16 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_16 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_16 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_17 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_17 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_17 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_17 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_18 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_18 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_18 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_18 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_19 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_19 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_19 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_19 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_2 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_2 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_2 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_2 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_20 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_20 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_20 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_20 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_21 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_21 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_21 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_21 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_22 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_22 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_22 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_22 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_23 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_23 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_23 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_23 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_24 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_24 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_24 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_24 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_25 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_25 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_25 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_25 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_26 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_26 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_26 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_26 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_27 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_27 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_27 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_27 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_28 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_28 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_28 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_28 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_29 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_29 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_29 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_29 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_3 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_3 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_3 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_3 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_30 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_30 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_30 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_30 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_31 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_31 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_31 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_31 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_32 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_32 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_32 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_32 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_33 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_33 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_33 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_33 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_34 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_34 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_34 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_34 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_35 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_35 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_35 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_35 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_36 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_36 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_36 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_36 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_37 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_37 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_37 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_37 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_38 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_38 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_38 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_38 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_39 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_39 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_39 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_39 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_4 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_4 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_4 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_4 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_40 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_40 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_40 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_40 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_41 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_41 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_41 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_41 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_42 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_42 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_42 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_42 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_43 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_43 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_43 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_43 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_44 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_44 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_44 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_44 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_45 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_45 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_45 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_45 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_46 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_46 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_46 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_46 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_47 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_47 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_47 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_47 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_48 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_48 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_48 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_48 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_49 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_49 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_49 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_49 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_5 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_5 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_5 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_5 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_50 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_50 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_50 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_50 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_51 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_51 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_51 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_51 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_52 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_52 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_52 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_52 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_53 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_53 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_53 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_53 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_54 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_54 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_54 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_54 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_55 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_55 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_55 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_55 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_56 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_56 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_56 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_56 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_57 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_57 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_57 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_57 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_58 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_58 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_58 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_58 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_59 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_59 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_59 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_59 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_6 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_6 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_6 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_6 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_60 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_60 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_60 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_60 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_61 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_61 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_61 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_61 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_62 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_62 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_62 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_62 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_63 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_63 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_63 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_63 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_64 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_64 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_64 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_64 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_65 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_65 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_65 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_65 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_66 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_66 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_66 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_66 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_67 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_67 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_67 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_67 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_68 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_68 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_68 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_68 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_69 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_69 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_69 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_69 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_7 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_7 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_7 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_7 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_70 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_70 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_70 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_70 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_71 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_71 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_71 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_71 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_72 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_72 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_72 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_72 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_73 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_73 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_73 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_73 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_74 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_74 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_74 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_74 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_75 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_75 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_75 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_75 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_76 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_76 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_76 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_76 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_77 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_77 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_77 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_77 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_78 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_78 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_78 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_78 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_79 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_79 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_79 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_79 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_8 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_8 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_8 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_8 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_80 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_80 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_80 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_80 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_81 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_81 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_81 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_81 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_82 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_82 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_82 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_82 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_83 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_83 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_83 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_83 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_84 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_84 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_84 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_84 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_85 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_85 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_85 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_85 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_86 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_86 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_86 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_86 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_87 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_87 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_87 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_87 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_88 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_88 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_88 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_88 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_89 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_89 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_89 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_89 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_9 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_9 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_9 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_9 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_90 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_90 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_90 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_90 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_91 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_91 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_91 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_91 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_92 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_92 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_92 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_92 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_93 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_93 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_93 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_93 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_94 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_94 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_94 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_94 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_95 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_95 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_95 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_95 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_96 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_96 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_96 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_96 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_97 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_97 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_97 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_97 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_98 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_98 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_98 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_98 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: order_history_99 create_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_99 ALTER COLUMN create_time SET DEFAULT now();


--
-- Name: order_history_99 finish_time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.order_history_99 ALTER COLUMN finish_time SET DEFAULT now();


--
-- Name: user_deal_history_0 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_0 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_1 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_1 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_10 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_10 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_11 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_11 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_12 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_12 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_13 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_13 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_14 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_14 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_15 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_15 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_16 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_16 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_17 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_17 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_18 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_18 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_19 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_19 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_2 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_2 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_20 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_20 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_21 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_21 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_22 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_22 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_23 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_23 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_24 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_24 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_25 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_25 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_26 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_26 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_27 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_27 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_28 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_28 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_29 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_29 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_3 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_3 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_30 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_30 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_31 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_31 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_32 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_32 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_33 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_33 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_34 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_34 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_35 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_35 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_36 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_36 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_37 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_37 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_38 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_38 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_39 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_39 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_4 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_4 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_40 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_40 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_41 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_41 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_42 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_42 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_43 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_43 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_44 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_44 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_45 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_45 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_46 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_46 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_47 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_47 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_48 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_48 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_49 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_49 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_5 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_5 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_50 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_50 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_51 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_51 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_52 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_52 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_53 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_53 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_54 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_54 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_55 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_55 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_56 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_56 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_57 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_57 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_58 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_58 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_59 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_59 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_6 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_6 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_60 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_60 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_61 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_61 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_62 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_62 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_63 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_63 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_64 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_64 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_65 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_65 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_66 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_66 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_67 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_67 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_68 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_68 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_69 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_69 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_7 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_7 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_70 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_70 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_71 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_71 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_72 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_72 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_73 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_73 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_74 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_74 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_75 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_75 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_76 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_76 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_77 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_77 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_78 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_78 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_79 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_79 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_8 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_8 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_80 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_80 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_81 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_81 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_82 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_82 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_83 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_83 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_84 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_84 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_85 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_85 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_86 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_86 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_87 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_87 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_88 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_88 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_89 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_89 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_9 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_9 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_90 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_90 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_91 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_91 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_92 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_92 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_93 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_93 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_94 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_94 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_95 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_95 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_96 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_96 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_97 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_97 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_98 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_98 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: user_deal_history_99 time; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_deal_history_99 ALTER COLUMN "time" SET DEFAULT now();


--
-- Name: SCHEMA public; Type: ACL; Schema: -; Owner: postgres
--

GRANT USAGE ON SCHEMA public TO readhistory;
GRANT USAGE ON SCHEMA public TO matchengine;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO matchengine;

--
-- Name: TABLE balance_history; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history TO matchengine;


--
-- Name: TABLE balance_history_0; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_0 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_0 TO matchengine;


--
-- Name: TABLE balance_history_1; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_1 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_1 TO matchengine;


--
-- Name: TABLE balance_history_10; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_10 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_10 TO matchengine;


--
-- Name: TABLE balance_history_11; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_11 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_11 TO matchengine;


--
-- Name: TABLE balance_history_12; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_12 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_12 TO matchengine;


--
-- Name: TABLE balance_history_13; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_13 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_13 TO matchengine;


--
-- Name: TABLE balance_history_14; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_14 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_14 TO matchengine;


--
-- Name: TABLE balance_history_15; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_15 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_15 TO matchengine;


--
-- Name: TABLE balance_history_16; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_16 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_16 TO matchengine;


--
-- Name: TABLE balance_history_17; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_17 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_17 TO matchengine;


--
-- Name: TABLE balance_history_18; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_18 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_18 TO matchengine;


--
-- Name: TABLE balance_history_19; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_19 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_19 TO matchengine;


--
-- Name: TABLE balance_history_2; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_2 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_2 TO matchengine;


--
-- Name: TABLE balance_history_20; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_20 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_20 TO matchengine;


--
-- Name: TABLE balance_history_21; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_21 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_21 TO matchengine;


--
-- Name: TABLE balance_history_22; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_22 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_22 TO matchengine;


--
-- Name: TABLE balance_history_23; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_23 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_23 TO matchengine;


--
-- Name: TABLE balance_history_24; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_24 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_24 TO matchengine;


--
-- Name: TABLE balance_history_25; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_25 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_25 TO matchengine;


--
-- Name: TABLE balance_history_26; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_26 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_26 TO matchengine;


--
-- Name: TABLE balance_history_27; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_27 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_27 TO matchengine;


--
-- Name: TABLE balance_history_28; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_28 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_28 TO matchengine;


--
-- Name: TABLE balance_history_29; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_29 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_29 TO matchengine;


--
-- Name: TABLE balance_history_3; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_3 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_3 TO matchengine;


--
-- Name: TABLE balance_history_30; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_30 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_30 TO matchengine;


--
-- Name: TABLE balance_history_31; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_31 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_31 TO matchengine;


--
-- Name: TABLE balance_history_32; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_32 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_32 TO matchengine;


--
-- Name: TABLE balance_history_33; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_33 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_33 TO matchengine;


--
-- Name: TABLE balance_history_34; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_34 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_34 TO matchengine;


--
-- Name: TABLE balance_history_35; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_35 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_35 TO matchengine;


--
-- Name: TABLE balance_history_36; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_36 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_36 TO matchengine;


--
-- Name: TABLE balance_history_37; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_37 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_37 TO matchengine;


--
-- Name: TABLE balance_history_38; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_38 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_38 TO matchengine;


--
-- Name: TABLE balance_history_39; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_39 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_39 TO matchengine;


--
-- Name: TABLE balance_history_4; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_4 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_4 TO matchengine;


--
-- Name: TABLE balance_history_40; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_40 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_40 TO matchengine;


--
-- Name: TABLE balance_history_41; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_41 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_41 TO matchengine;


--
-- Name: TABLE balance_history_42; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_42 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_42 TO matchengine;


--
-- Name: TABLE balance_history_43; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_43 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_43 TO matchengine;


--
-- Name: TABLE balance_history_44; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_44 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_44 TO matchengine;


--
-- Name: TABLE balance_history_45; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_45 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_45 TO matchengine;


--
-- Name: TABLE balance_history_46; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_46 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_46 TO matchengine;


--
-- Name: TABLE balance_history_47; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_47 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_47 TO matchengine;


--
-- Name: TABLE balance_history_48; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_48 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_48 TO matchengine;


--
-- Name: TABLE balance_history_49; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_49 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_49 TO matchengine;


--
-- Name: TABLE balance_history_5; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_5 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_5 TO matchengine;


--
-- Name: TABLE balance_history_50; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_50 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_50 TO matchengine;


--
-- Name: TABLE balance_history_51; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_51 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_51 TO matchengine;


--
-- Name: TABLE balance_history_52; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_52 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_52 TO matchengine;


--
-- Name: TABLE balance_history_53; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_53 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_53 TO matchengine;


--
-- Name: TABLE balance_history_54; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_54 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_54 TO matchengine;


--
-- Name: TABLE balance_history_55; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_55 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_55 TO matchengine;


--
-- Name: TABLE balance_history_56; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_56 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_56 TO matchengine;


--
-- Name: TABLE balance_history_57; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_57 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_57 TO matchengine;


--
-- Name: TABLE balance_history_58; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_58 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_58 TO matchengine;


--
-- Name: TABLE balance_history_59; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_59 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_59 TO matchengine;


--
-- Name: TABLE balance_history_6; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_6 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_6 TO matchengine;


--
-- Name: TABLE balance_history_60; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_60 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_60 TO matchengine;


--
-- Name: TABLE balance_history_61; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_61 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_61 TO matchengine;


--
-- Name: TABLE balance_history_62; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_62 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_62 TO matchengine;


--
-- Name: TABLE balance_history_63; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_63 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_63 TO matchengine;


--
-- Name: TABLE balance_history_64; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_64 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_64 TO matchengine;


--
-- Name: TABLE balance_history_65; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_65 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_65 TO matchengine;


--
-- Name: TABLE balance_history_66; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_66 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_66 TO matchengine;


--
-- Name: TABLE balance_history_67; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_67 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_67 TO matchengine;


--
-- Name: TABLE balance_history_68; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_68 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_68 TO matchengine;


--
-- Name: TABLE balance_history_69; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_69 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_69 TO matchengine;


--
-- Name: TABLE balance_history_7; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_7 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_7 TO matchengine;


--
-- Name: TABLE balance_history_70; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_70 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_70 TO matchengine;


--
-- Name: TABLE balance_history_71; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_71 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_71 TO matchengine;


--
-- Name: TABLE balance_history_72; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_72 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_72 TO matchengine;


--
-- Name: TABLE balance_history_73; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_73 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_73 TO matchengine;


--
-- Name: TABLE balance_history_74; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_74 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_74 TO matchengine;


--
-- Name: TABLE balance_history_75; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_75 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_75 TO matchengine;


--
-- Name: TABLE balance_history_76; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_76 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_76 TO matchengine;


--
-- Name: TABLE balance_history_77; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_77 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_77 TO matchengine;


--
-- Name: TABLE balance_history_78; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_78 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_78 TO matchengine;


--
-- Name: TABLE balance_history_79; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_79 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_79 TO matchengine;


--
-- Name: TABLE balance_history_8; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_8 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_8 TO matchengine;


--
-- Name: TABLE balance_history_80; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_80 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_80 TO matchengine;


--
-- Name: TABLE balance_history_81; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_81 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_81 TO matchengine;


--
-- Name: TABLE balance_history_82; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_82 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_82 TO matchengine;


--
-- Name: TABLE balance_history_83; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_83 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_83 TO matchengine;


--
-- Name: TABLE balance_history_84; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_84 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_84 TO matchengine;


--
-- Name: TABLE balance_history_85; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_85 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_85 TO matchengine;


--
-- Name: TABLE balance_history_86; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_86 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_86 TO matchengine;


--
-- Name: TABLE balance_history_87; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_87 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_87 TO matchengine;


--
-- Name: TABLE balance_history_88; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_88 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_88 TO matchengine;


--
-- Name: TABLE balance_history_89; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_89 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_89 TO matchengine;


--
-- Name: TABLE balance_history_9; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_9 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_9 TO matchengine;


--
-- Name: TABLE balance_history_90; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_90 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_90 TO matchengine;


--
-- Name: TABLE balance_history_91; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_91 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_91 TO matchengine;


--
-- Name: TABLE balance_history_92; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_92 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_92 TO matchengine;


--
-- Name: TABLE balance_history_93; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_93 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_93 TO matchengine;


--
-- Name: TABLE balance_history_94; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_94 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_94 TO matchengine;


--
-- Name: TABLE balance_history_95; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_95 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_95 TO matchengine;


--
-- Name: TABLE balance_history_96; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_96 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_96 TO matchengine;


--
-- Name: TABLE balance_history_97; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_97 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_97 TO matchengine;


--
-- Name: TABLE balance_history_98; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_98 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_98 TO matchengine;


--
-- Name: TABLE balance_history_99; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.balance_history_99 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.balance_history_99 TO matchengine;


--
-- Name: TABLE deal_history; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history TO matchengine;


--
-- Name: TABLE deal_history_0; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_0 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_0 TO matchengine;


--
-- Name: TABLE deal_history_1; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_1 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_1 TO matchengine;


--
-- Name: TABLE deal_history_10; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_10 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_10 TO matchengine;


--
-- Name: TABLE deal_history_11; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_11 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_11 TO matchengine;


--
-- Name: TABLE deal_history_12; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_12 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_12 TO matchengine;


--
-- Name: TABLE deal_history_13; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_13 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_13 TO matchengine;


--
-- Name: TABLE deal_history_14; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_14 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_14 TO matchengine;


--
-- Name: TABLE deal_history_15; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_15 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_15 TO matchengine;


--
-- Name: TABLE deal_history_16; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_16 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_16 TO matchengine;


--
-- Name: TABLE deal_history_17; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_17 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_17 TO matchengine;


--
-- Name: TABLE deal_history_18; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_18 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_18 TO matchengine;


--
-- Name: TABLE deal_history_19; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_19 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_19 TO matchengine;


--
-- Name: TABLE deal_history_2; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_2 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_2 TO matchengine;


--
-- Name: TABLE deal_history_20; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_20 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_20 TO matchengine;


--
-- Name: TABLE deal_history_21; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_21 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_21 TO matchengine;


--
-- Name: TABLE deal_history_22; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_22 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_22 TO matchengine;


--
-- Name: TABLE deal_history_23; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_23 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_23 TO matchengine;


--
-- Name: TABLE deal_history_24; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_24 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_24 TO matchengine;


--
-- Name: TABLE deal_history_25; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_25 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_25 TO matchengine;


--
-- Name: TABLE deal_history_26; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_26 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_26 TO matchengine;


--
-- Name: TABLE deal_history_27; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_27 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_27 TO matchengine;


--
-- Name: TABLE deal_history_28; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_28 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_28 TO matchengine;


--
-- Name: TABLE deal_history_29; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_29 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_29 TO matchengine;


--
-- Name: TABLE deal_history_3; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_3 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_3 TO matchengine;


--
-- Name: TABLE deal_history_30; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_30 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_30 TO matchengine;


--
-- Name: TABLE deal_history_31; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_31 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_31 TO matchengine;


--
-- Name: TABLE deal_history_32; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_32 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_32 TO matchengine;


--
-- Name: TABLE deal_history_33; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_33 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_33 TO matchengine;


--
-- Name: TABLE deal_history_34; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_34 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_34 TO matchengine;


--
-- Name: TABLE deal_history_35; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_35 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_35 TO matchengine;


--
-- Name: TABLE deal_history_36; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_36 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_36 TO matchengine;


--
-- Name: TABLE deal_history_37; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_37 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_37 TO matchengine;


--
-- Name: TABLE deal_history_38; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_38 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_38 TO matchengine;


--
-- Name: TABLE deal_history_39; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_39 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_39 TO matchengine;


--
-- Name: TABLE deal_history_4; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_4 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_4 TO matchengine;


--
-- Name: TABLE deal_history_40; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_40 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_40 TO matchengine;


--
-- Name: TABLE deal_history_41; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_41 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_41 TO matchengine;


--
-- Name: TABLE deal_history_42; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_42 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_42 TO matchengine;


--
-- Name: TABLE deal_history_43; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_43 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_43 TO matchengine;


--
-- Name: TABLE deal_history_44; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_44 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_44 TO matchengine;


--
-- Name: TABLE deal_history_45; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_45 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_45 TO matchengine;


--
-- Name: TABLE deal_history_46; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_46 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_46 TO matchengine;


--
-- Name: TABLE deal_history_47; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_47 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_47 TO matchengine;


--
-- Name: TABLE deal_history_48; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_48 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_48 TO matchengine;


--
-- Name: TABLE deal_history_49; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_49 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_49 TO matchengine;


--
-- Name: TABLE deal_history_5; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_5 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_5 TO matchengine;


--
-- Name: TABLE deal_history_50; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_50 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_50 TO matchengine;


--
-- Name: TABLE deal_history_51; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_51 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_51 TO matchengine;


--
-- Name: TABLE deal_history_52; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_52 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_52 TO matchengine;


--
-- Name: TABLE deal_history_53; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_53 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_53 TO matchengine;


--
-- Name: TABLE deal_history_54; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_54 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_54 TO matchengine;


--
-- Name: TABLE deal_history_55; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_55 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_55 TO matchengine;


--
-- Name: TABLE deal_history_56; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_56 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_56 TO matchengine;


--
-- Name: TABLE deal_history_57; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_57 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_57 TO matchengine;


--
-- Name: TABLE deal_history_58; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_58 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_58 TO matchengine;


--
-- Name: TABLE deal_history_59; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_59 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_59 TO matchengine;


--
-- Name: TABLE deal_history_6; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_6 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_6 TO matchengine;


--
-- Name: TABLE deal_history_60; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_60 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_60 TO matchengine;


--
-- Name: TABLE deal_history_61; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_61 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_61 TO matchengine;


--
-- Name: TABLE deal_history_62; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_62 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_62 TO matchengine;


--
-- Name: TABLE deal_history_63; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_63 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_63 TO matchengine;


--
-- Name: TABLE deal_history_64; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_64 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_64 TO matchengine;


--
-- Name: TABLE deal_history_65; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_65 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_65 TO matchengine;


--
-- Name: TABLE deal_history_66; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_66 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_66 TO matchengine;


--
-- Name: TABLE deal_history_67; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_67 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_67 TO matchengine;


--
-- Name: TABLE deal_history_68; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_68 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_68 TO matchengine;


--
-- Name: TABLE deal_history_69; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_69 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_69 TO matchengine;


--
-- Name: TABLE deal_history_7; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_7 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_7 TO matchengine;


--
-- Name: TABLE deal_history_70; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_70 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_70 TO matchengine;


--
-- Name: TABLE deal_history_71; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_71 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_71 TO matchengine;


--
-- Name: TABLE deal_history_72; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_72 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_72 TO matchengine;


--
-- Name: TABLE deal_history_73; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_73 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_73 TO matchengine;


--
-- Name: TABLE deal_history_74; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_74 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_74 TO matchengine;


--
-- Name: TABLE deal_history_75; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_75 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_75 TO matchengine;


--
-- Name: TABLE deal_history_76; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_76 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_76 TO matchengine;


--
-- Name: TABLE deal_history_77; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_77 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_77 TO matchengine;


--
-- Name: TABLE deal_history_78; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_78 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_78 TO matchengine;


--
-- Name: TABLE deal_history_79; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_79 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_79 TO matchengine;


--
-- Name: TABLE deal_history_8; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_8 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_8 TO matchengine;


--
-- Name: TABLE deal_history_80; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_80 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_80 TO matchengine;


--
-- Name: TABLE deal_history_81; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_81 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_81 TO matchengine;


--
-- Name: TABLE deal_history_82; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_82 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_82 TO matchengine;


--
-- Name: TABLE deal_history_83; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_83 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_83 TO matchengine;


--
-- Name: TABLE deal_history_84; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_84 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_84 TO matchengine;


--
-- Name: TABLE deal_history_85; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_85 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_85 TO matchengine;


--
-- Name: TABLE deal_history_86; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_86 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_86 TO matchengine;


--
-- Name: TABLE deal_history_87; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_87 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_87 TO matchengine;


--
-- Name: TABLE deal_history_88; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_88 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_88 TO matchengine;


--
-- Name: TABLE deal_history_89; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_89 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_89 TO matchengine;


--
-- Name: TABLE deal_history_9; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_9 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_9 TO matchengine;


--
-- Name: TABLE deal_history_90; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_90 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_90 TO matchengine;


--
-- Name: TABLE deal_history_91; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_91 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_91 TO matchengine;


--
-- Name: TABLE deal_history_92; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_92 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_92 TO matchengine;


--
-- Name: TABLE deal_history_93; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_93 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_93 TO matchengine;


--
-- Name: TABLE deal_history_94; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_94 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_94 TO matchengine;


--
-- Name: TABLE deal_history_95; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_95 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_95 TO matchengine;


--
-- Name: TABLE deal_history_96; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_96 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_96 TO matchengine;


--
-- Name: TABLE deal_history_97; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_97 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_97 TO matchengine;


--
-- Name: TABLE deal_history_98; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_98 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_98 TO matchengine;


--
-- Name: TABLE deal_history_99; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.deal_history_99 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.deal_history_99 TO matchengine;



--
-- Name: TABLE order_detail; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail TO matchengine;


--
-- Name: TABLE order_detail_0; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_0 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_0 TO matchengine;


--
-- Name: TABLE order_detail_1; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_1 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_1 TO matchengine;


--
-- Name: TABLE order_detail_10; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_10 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_10 TO matchengine;


--
-- Name: TABLE order_detail_11; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_11 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_11 TO matchengine;


--
-- Name: TABLE order_detail_12; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_12 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_12 TO matchengine;


--
-- Name: TABLE order_detail_13; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_13 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_13 TO matchengine;


--
-- Name: TABLE order_detail_14; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_14 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_14 TO matchengine;


--
-- Name: TABLE order_detail_15; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_15 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_15 TO matchengine;


--
-- Name: TABLE order_detail_16; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_16 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_16 TO matchengine;


--
-- Name: TABLE order_detail_17; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_17 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_17 TO matchengine;


--
-- Name: TABLE order_detail_18; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_18 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_18 TO matchengine;


--
-- Name: TABLE order_detail_19; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_19 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_19 TO matchengine;


--
-- Name: TABLE order_detail_2; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_2 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_2 TO matchengine;


--
-- Name: TABLE order_detail_20; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_20 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_20 TO matchengine;


--
-- Name: TABLE order_detail_21; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_21 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_21 TO matchengine;


--
-- Name: TABLE order_detail_22; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_22 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_22 TO matchengine;


--
-- Name: TABLE order_detail_23; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_23 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_23 TO matchengine;


--
-- Name: TABLE order_detail_24; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_24 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_24 TO matchengine;


--
-- Name: TABLE order_detail_25; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_25 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_25 TO matchengine;


--
-- Name: TABLE order_detail_26; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_26 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_26 TO matchengine;


--
-- Name: TABLE order_detail_27; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_27 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_27 TO matchengine;


--
-- Name: TABLE order_detail_28; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_28 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_28 TO matchengine;


--
-- Name: TABLE order_detail_29; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_29 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_29 TO matchengine;


--
-- Name: TABLE order_detail_3; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_3 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_3 TO matchengine;


--
-- Name: TABLE order_detail_30; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_30 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_30 TO matchengine;


--
-- Name: TABLE order_detail_31; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_31 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_31 TO matchengine;


--
-- Name: TABLE order_detail_32; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_32 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_32 TO matchengine;


--
-- Name: TABLE order_detail_33; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_33 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_33 TO matchengine;


--
-- Name: TABLE order_detail_34; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_34 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_34 TO matchengine;


--
-- Name: TABLE order_detail_35; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_35 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_35 TO matchengine;


--
-- Name: TABLE order_detail_36; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_36 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_36 TO matchengine;


--
-- Name: TABLE order_detail_37; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_37 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_37 TO matchengine;


--
-- Name: TABLE order_detail_38; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_38 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_38 TO matchengine;


--
-- Name: TABLE order_detail_39; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_39 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_39 TO matchengine;


--
-- Name: TABLE order_detail_4; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_4 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_4 TO matchengine;


--
-- Name: TABLE order_detail_40; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_40 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_40 TO matchengine;


--
-- Name: TABLE order_detail_41; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_41 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_41 TO matchengine;


--
-- Name: TABLE order_detail_42; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_42 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_42 TO matchengine;


--
-- Name: TABLE order_detail_43; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_43 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_43 TO matchengine;


--
-- Name: TABLE order_detail_44; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_44 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_44 TO matchengine;


--
-- Name: TABLE order_detail_45; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_45 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_45 TO matchengine;


--
-- Name: TABLE order_detail_46; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_46 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_46 TO matchengine;


--
-- Name: TABLE order_detail_47; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_47 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_47 TO matchengine;


--
-- Name: TABLE order_detail_48; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_48 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_48 TO matchengine;


--
-- Name: TABLE order_detail_49; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_49 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_49 TO matchengine;


--
-- Name: TABLE order_detail_5; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_5 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_5 TO matchengine;


--
-- Name: TABLE order_detail_50; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_50 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_50 TO matchengine;


--
-- Name: TABLE order_detail_51; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_51 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_51 TO matchengine;


--
-- Name: TABLE order_detail_52; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_52 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_52 TO matchengine;


--
-- Name: TABLE order_detail_53; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_53 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_53 TO matchengine;


--
-- Name: TABLE order_detail_54; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_54 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_54 TO matchengine;


--
-- Name: TABLE order_detail_55; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_55 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_55 TO matchengine;


--
-- Name: TABLE order_detail_56; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_56 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_56 TO matchengine;


--
-- Name: TABLE order_detail_57; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_57 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_57 TO matchengine;


--
-- Name: TABLE order_detail_58; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_58 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_58 TO matchengine;


--
-- Name: TABLE order_detail_59; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_59 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_59 TO matchengine;


--
-- Name: TABLE order_detail_6; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_6 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_6 TO matchengine;


--
-- Name: TABLE order_detail_60; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_60 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_60 TO matchengine;


--
-- Name: TABLE order_detail_61; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_61 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_61 TO matchengine;


--
-- Name: TABLE order_detail_62; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_62 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_62 TO matchengine;


--
-- Name: TABLE order_detail_63; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_63 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_63 TO matchengine;


--
-- Name: TABLE order_detail_64; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_64 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_64 TO matchengine;


--
-- Name: TABLE order_detail_65; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_65 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_65 TO matchengine;


--
-- Name: TABLE order_detail_66; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_66 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_66 TO matchengine;


--
-- Name: TABLE order_detail_67; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_67 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_67 TO matchengine;


--
-- Name: TABLE order_detail_68; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_68 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_68 TO matchengine;


--
-- Name: TABLE order_detail_69; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_69 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_69 TO matchengine;


--
-- Name: TABLE order_detail_7; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_7 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_7 TO matchengine;


--
-- Name: TABLE order_detail_70; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_70 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_70 TO matchengine;


--
-- Name: TABLE order_detail_71; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_71 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_71 TO matchengine;


--
-- Name: TABLE order_detail_72; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_72 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_72 TO matchengine;


--
-- Name: TABLE order_detail_73; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_73 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_73 TO matchengine;


--
-- Name: TABLE order_detail_74; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_74 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_74 TO matchengine;


--
-- Name: TABLE order_detail_75; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_75 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_75 TO matchengine;


--
-- Name: TABLE order_detail_76; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_76 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_76 TO matchengine;


--
-- Name: TABLE order_detail_77; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_77 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_77 TO matchengine;


--
-- Name: TABLE order_detail_78; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_78 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_78 TO matchengine;


--
-- Name: TABLE order_detail_79; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_79 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_79 TO matchengine;


--
-- Name: TABLE order_detail_8; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_8 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_8 TO matchengine;


--
-- Name: TABLE order_detail_80; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_80 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_80 TO matchengine;


--
-- Name: TABLE order_detail_81; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_81 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_81 TO matchengine;


--
-- Name: TABLE order_detail_82; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_82 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_82 TO matchengine;


--
-- Name: TABLE order_detail_83; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_83 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_83 TO matchengine;


--
-- Name: TABLE order_detail_84; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_84 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_84 TO matchengine;


--
-- Name: TABLE order_detail_85; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_85 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_85 TO matchengine;


--
-- Name: TABLE order_detail_86; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_86 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_86 TO matchengine;


--
-- Name: TABLE order_detail_87; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_87 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_87 TO matchengine;


--
-- Name: TABLE order_detail_88; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_88 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_88 TO matchengine;


--
-- Name: TABLE order_detail_89; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_89 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_89 TO matchengine;


--
-- Name: TABLE order_detail_9; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_9 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_9 TO matchengine;


--
-- Name: TABLE order_detail_90; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_90 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_90 TO matchengine;


--
-- Name: TABLE order_detail_91; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_91 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_91 TO matchengine;


--
-- Name: TABLE order_detail_92; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_92 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_92 TO matchengine;


--
-- Name: TABLE order_detail_93; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_93 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_93 TO matchengine;


--
-- Name: TABLE order_detail_94; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_94 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_94 TO matchengine;


--
-- Name: TABLE order_detail_95; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_95 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_95 TO matchengine;


--
-- Name: TABLE order_detail_96; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_96 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_96 TO matchengine;


--
-- Name: TABLE order_detail_97; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_97 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_97 TO matchengine;


--
-- Name: TABLE order_detail_98; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_98 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_98 TO matchengine;


--
-- Name: TABLE order_detail_99; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_detail_99 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_detail_99 TO matchengine;


--
-- Name: TABLE order_history; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history TO matchengine;


--
-- Name: TABLE order_history_0; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_0 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_0 TO matchengine;


--
-- Name: TABLE order_history_1; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_1 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_1 TO matchengine;


--
-- Name: TABLE order_history_10; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_10 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_10 TO matchengine;


--
-- Name: TABLE order_history_11; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_11 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_11 TO matchengine;


--
-- Name: TABLE order_history_12; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_12 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_12 TO matchengine;


--
-- Name: TABLE order_history_13; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_13 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_13 TO matchengine;


--
-- Name: TABLE order_history_14; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_14 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_14 TO matchengine;


--
-- Name: TABLE order_history_15; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_15 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_15 TO matchengine;


--
-- Name: TABLE order_history_16; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_16 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_16 TO matchengine;


--
-- Name: TABLE order_history_17; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_17 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_17 TO matchengine;


--
-- Name: TABLE order_history_18; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_18 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_18 TO matchengine;


--
-- Name: TABLE order_history_19; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_19 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_19 TO matchengine;


--
-- Name: TABLE order_history_2; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_2 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_2 TO matchengine;


--
-- Name: TABLE order_history_20; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_20 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_20 TO matchengine;


--
-- Name: TABLE order_history_21; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_21 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_21 TO matchengine;


--
-- Name: TABLE order_history_22; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_22 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_22 TO matchengine;


--
-- Name: TABLE order_history_23; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_23 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_23 TO matchengine;


--
-- Name: TABLE order_history_24; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_24 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_24 TO matchengine;


--
-- Name: TABLE order_history_25; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_25 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_25 TO matchengine;


--
-- Name: TABLE order_history_26; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_26 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_26 TO matchengine;


--
-- Name: TABLE order_history_27; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_27 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_27 TO matchengine;


--
-- Name: TABLE order_history_28; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_28 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_28 TO matchengine;


--
-- Name: TABLE order_history_29; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_29 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_29 TO matchengine;


--
-- Name: TABLE order_history_3; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_3 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_3 TO matchengine;


--
-- Name: TABLE order_history_30; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_30 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_30 TO matchengine;


--
-- Name: TABLE order_history_31; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_31 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_31 TO matchengine;


--
-- Name: TABLE order_history_32; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_32 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_32 TO matchengine;


--
-- Name: TABLE order_history_33; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_33 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_33 TO matchengine;


--
-- Name: TABLE order_history_34; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_34 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_34 TO matchengine;


--
-- Name: TABLE order_history_35; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_35 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_35 TO matchengine;


--
-- Name: TABLE order_history_36; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_36 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_36 TO matchengine;


--
-- Name: TABLE order_history_37; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_37 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_37 TO matchengine;


--
-- Name: TABLE order_history_38; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_38 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_38 TO matchengine;


--
-- Name: TABLE order_history_39; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_39 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_39 TO matchengine;


--
-- Name: TABLE order_history_4; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_4 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_4 TO matchengine;


--
-- Name: TABLE order_history_40; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_40 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_40 TO matchengine;


--
-- Name: TABLE order_history_41; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_41 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_41 TO matchengine;


--
-- Name: TABLE order_history_42; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_42 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_42 TO matchengine;


--
-- Name: TABLE order_history_43; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_43 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_43 TO matchengine;


--
-- Name: TABLE order_history_44; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_44 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_44 TO matchengine;


--
-- Name: TABLE order_history_45; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_45 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_45 TO matchengine;


--
-- Name: TABLE order_history_46; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_46 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_46 TO matchengine;


--
-- Name: TABLE order_history_47; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_47 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_47 TO matchengine;


--
-- Name: TABLE order_history_48; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_48 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_48 TO matchengine;


--
-- Name: TABLE order_history_49; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_49 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_49 TO matchengine;


--
-- Name: TABLE order_history_5; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_5 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_5 TO matchengine;


--
-- Name: TABLE order_history_50; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_50 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_50 TO matchengine;


--
-- Name: TABLE order_history_51; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_51 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_51 TO matchengine;


--
-- Name: TABLE order_history_52; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_52 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_52 TO matchengine;


--
-- Name: TABLE order_history_53; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_53 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_53 TO matchengine;


--
-- Name: TABLE order_history_54; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_54 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_54 TO matchengine;


--
-- Name: TABLE order_history_55; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_55 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_55 TO matchengine;


--
-- Name: TABLE order_history_56; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_56 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_56 TO matchengine;


--
-- Name: TABLE order_history_57; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_57 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_57 TO matchengine;


--
-- Name: TABLE order_history_58; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_58 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_58 TO matchengine;


--
-- Name: TABLE order_history_59; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_59 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_59 TO matchengine;


--
-- Name: TABLE order_history_6; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_6 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_6 TO matchengine;


--
-- Name: TABLE order_history_60; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_60 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_60 TO matchengine;


--
-- Name: TABLE order_history_61; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_61 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_61 TO matchengine;


--
-- Name: TABLE order_history_62; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_62 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_62 TO matchengine;


--
-- Name: TABLE order_history_63; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_63 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_63 TO matchengine;


--
-- Name: TABLE order_history_64; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_64 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_64 TO matchengine;


--
-- Name: TABLE order_history_65; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_65 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_65 TO matchengine;


--
-- Name: TABLE order_history_66; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_66 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_66 TO matchengine;


--
-- Name: TABLE order_history_67; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_67 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_67 TO matchengine;


--
-- Name: TABLE order_history_68; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_68 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_68 TO matchengine;


--
-- Name: TABLE order_history_69; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_69 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_69 TO matchengine;


--
-- Name: TABLE order_history_7; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_7 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_7 TO matchengine;


--
-- Name: TABLE order_history_70; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_70 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_70 TO matchengine;


--
-- Name: TABLE order_history_71; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_71 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_71 TO matchengine;


--
-- Name: TABLE order_history_72; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_72 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_72 TO matchengine;


--
-- Name: TABLE order_history_73; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_73 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_73 TO matchengine;


--
-- Name: TABLE order_history_74; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_74 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_74 TO matchengine;


--
-- Name: TABLE order_history_75; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_75 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_75 TO matchengine;


--
-- Name: TABLE order_history_76; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_76 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_76 TO matchengine;


--
-- Name: TABLE order_history_77; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_77 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_77 TO matchengine;


--
-- Name: TABLE order_history_78; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_78 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_78 TO matchengine;


--
-- Name: TABLE order_history_79; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_79 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_79 TO matchengine;


--
-- Name: TABLE order_history_8; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_8 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_8 TO matchengine;


--
-- Name: TABLE order_history_80; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_80 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_80 TO matchengine;


--
-- Name: TABLE order_history_81; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_81 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_81 TO matchengine;


--
-- Name: TABLE order_history_82; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_82 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_82 TO matchengine;


--
-- Name: TABLE order_history_83; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_83 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_83 TO matchengine;


--
-- Name: TABLE order_history_84; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_84 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_84 TO matchengine;


--
-- Name: TABLE order_history_85; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_85 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_85 TO matchengine;


--
-- Name: TABLE order_history_86; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_86 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_86 TO matchengine;


--
-- Name: TABLE order_history_87; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_87 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_87 TO matchengine;


--
-- Name: TABLE order_history_88; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_88 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_88 TO matchengine;


--
-- Name: TABLE order_history_89; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_89 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_89 TO matchengine;


--
-- Name: TABLE order_history_9; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_9 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_9 TO matchengine;


--
-- Name: TABLE order_history_90; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_90 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_90 TO matchengine;


--
-- Name: TABLE order_history_91; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_91 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_91 TO matchengine;


--
-- Name: TABLE order_history_92; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_92 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_92 TO matchengine;


--
-- Name: TABLE order_history_93; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_93 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_93 TO matchengine;


--
-- Name: TABLE order_history_94; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_94 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_94 TO matchengine;


--
-- Name: TABLE order_history_95; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_95 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_95 TO matchengine;


--
-- Name: TABLE order_history_96; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_96 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_96 TO matchengine;


--
-- Name: TABLE order_history_97; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_97 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_97 TO matchengine;


--
-- Name: TABLE order_history_98; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_98 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_98 TO matchengine;


--
-- Name: TABLE order_history_99; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.order_history_99 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.order_history_99 TO matchengine;


--
-- Name: TABLE user_deal_history; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history TO matchengine;


--
-- Name: TABLE user_deal_history_0; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_0 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_0 TO matchengine;


--
-- Name: TABLE user_deal_history_1; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_1 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_1 TO matchengine;


--
-- Name: TABLE user_deal_history_10; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_10 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_10 TO matchengine;


--
-- Name: TABLE user_deal_history_11; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_11 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_11 TO matchengine;


--
-- Name: TABLE user_deal_history_12; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_12 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_12 TO matchengine;


--
-- Name: TABLE user_deal_history_13; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_13 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_13 TO matchengine;


--
-- Name: TABLE user_deal_history_14; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_14 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_14 TO matchengine;


--
-- Name: TABLE user_deal_history_15; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_15 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_15 TO matchengine;


--
-- Name: TABLE user_deal_history_16; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_16 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_16 TO matchengine;


--
-- Name: TABLE user_deal_history_17; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_17 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_17 TO matchengine;


--
-- Name: TABLE user_deal_history_18; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_18 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_18 TO matchengine;


--
-- Name: TABLE user_deal_history_19; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_19 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_19 TO matchengine;


--
-- Name: TABLE user_deal_history_2; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_2 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_2 TO matchengine;


--
-- Name: TABLE user_deal_history_20; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_20 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_20 TO matchengine;


--
-- Name: TABLE user_deal_history_21; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_21 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_21 TO matchengine;


--
-- Name: TABLE user_deal_history_22; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_22 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_22 TO matchengine;


--
-- Name: TABLE user_deal_history_23; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_23 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_23 TO matchengine;


--
-- Name: TABLE user_deal_history_24; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_24 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_24 TO matchengine;


--
-- Name: TABLE user_deal_history_25; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_25 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_25 TO matchengine;


--
-- Name: TABLE user_deal_history_26; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_26 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_26 TO matchengine;


--
-- Name: TABLE user_deal_history_27; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_27 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_27 TO matchengine;


--
-- Name: TABLE user_deal_history_28; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_28 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_28 TO matchengine;


--
-- Name: TABLE user_deal_history_29; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_29 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_29 TO matchengine;


--
-- Name: TABLE user_deal_history_3; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_3 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_3 TO matchengine;


--
-- Name: TABLE user_deal_history_30; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_30 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_30 TO matchengine;


--
-- Name: TABLE user_deal_history_31; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_31 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_31 TO matchengine;


--
-- Name: TABLE user_deal_history_32; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_32 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_32 TO matchengine;


--
-- Name: TABLE user_deal_history_33; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_33 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_33 TO matchengine;


--
-- Name: TABLE user_deal_history_34; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_34 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_34 TO matchengine;


--
-- Name: TABLE user_deal_history_35; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_35 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_35 TO matchengine;


--
-- Name: TABLE user_deal_history_36; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_36 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_36 TO matchengine;


--
-- Name: TABLE user_deal_history_37; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_37 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_37 TO matchengine;


--
-- Name: TABLE user_deal_history_38; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_38 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_38 TO matchengine;


--
-- Name: TABLE user_deal_history_39; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_39 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_39 TO matchengine;


--
-- Name: TABLE user_deal_history_4; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_4 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_4 TO matchengine;


--
-- Name: TABLE user_deal_history_40; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_40 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_40 TO matchengine;


--
-- Name: TABLE user_deal_history_41; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_41 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_41 TO matchengine;


--
-- Name: TABLE user_deal_history_42; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_42 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_42 TO matchengine;


--
-- Name: TABLE user_deal_history_43; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_43 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_43 TO matchengine;


--
-- Name: TABLE user_deal_history_44; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_44 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_44 TO matchengine;


--
-- Name: TABLE user_deal_history_45; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_45 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_45 TO matchengine;


--
-- Name: TABLE user_deal_history_46; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_46 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_46 TO matchengine;


--
-- Name: TABLE user_deal_history_47; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_47 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_47 TO matchengine;


--
-- Name: TABLE user_deal_history_48; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_48 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_48 TO matchengine;


--
-- Name: TABLE user_deal_history_49; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_49 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_49 TO matchengine;


--
-- Name: TABLE user_deal_history_5; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_5 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_5 TO matchengine;


--
-- Name: TABLE user_deal_history_50; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_50 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_50 TO matchengine;


--
-- Name: TABLE user_deal_history_51; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_51 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_51 TO matchengine;


--
-- Name: TABLE user_deal_history_52; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_52 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_52 TO matchengine;


--
-- Name: TABLE user_deal_history_53; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_53 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_53 TO matchengine;


--
-- Name: TABLE user_deal_history_54; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_54 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_54 TO matchengine;


--
-- Name: TABLE user_deal_history_55; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_55 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_55 TO matchengine;


--
-- Name: TABLE user_deal_history_56; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_56 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_56 TO matchengine;


--
-- Name: TABLE user_deal_history_57; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_57 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_57 TO matchengine;


--
-- Name: TABLE user_deal_history_58; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_58 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_58 TO matchengine;


--
-- Name: TABLE user_deal_history_59; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_59 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_59 TO matchengine;


--
-- Name: TABLE user_deal_history_6; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_6 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_6 TO matchengine;


--
-- Name: TABLE user_deal_history_60; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_60 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_60 TO matchengine;


--
-- Name: TABLE user_deal_history_61; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_61 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_61 TO matchengine;


--
-- Name: TABLE user_deal_history_62; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_62 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_62 TO matchengine;


--
-- Name: TABLE user_deal_history_63; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_63 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_63 TO matchengine;


--
-- Name: TABLE user_deal_history_64; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_64 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_64 TO matchengine;


--
-- Name: TABLE user_deal_history_65; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_65 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_65 TO matchengine;


--
-- Name: TABLE user_deal_history_66; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_66 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_66 TO matchengine;


--
-- Name: TABLE user_deal_history_67; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_67 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_67 TO matchengine;


--
-- Name: TABLE user_deal_history_68; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_68 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_68 TO matchengine;


--
-- Name: TABLE user_deal_history_69; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_69 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_69 TO matchengine;


--
-- Name: TABLE user_deal_history_7; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_7 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_7 TO matchengine;


--
-- Name: TABLE user_deal_history_70; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_70 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_70 TO matchengine;


--
-- Name: TABLE user_deal_history_71; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_71 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_71 TO matchengine;


--
-- Name: TABLE user_deal_history_72; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_72 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_72 TO matchengine;


--
-- Name: TABLE user_deal_history_73; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_73 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_73 TO matchengine;


--
-- Name: TABLE user_deal_history_74; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_74 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_74 TO matchengine;


--
-- Name: TABLE user_deal_history_75; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_75 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_75 TO matchengine;


--
-- Name: TABLE user_deal_history_76; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_76 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_76 TO matchengine;


--
-- Name: TABLE user_deal_history_77; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_77 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_77 TO matchengine;


--
-- Name: TABLE user_deal_history_78; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_78 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_78 TO matchengine;


--
-- Name: TABLE user_deal_history_79; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_79 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_79 TO matchengine;


--
-- Name: TABLE user_deal_history_8; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_8 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_8 TO matchengine;


--
-- Name: TABLE user_deal_history_80; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_80 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_80 TO matchengine;


--
-- Name: TABLE user_deal_history_81; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_81 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_81 TO matchengine;


--
-- Name: TABLE user_deal_history_82; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_82 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_82 TO matchengine;


--
-- Name: TABLE user_deal_history_83; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_83 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_83 TO matchengine;


--
-- Name: TABLE user_deal_history_84; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_84 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_84 TO matchengine;


--
-- Name: TABLE user_deal_history_85; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_85 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_85 TO matchengine;


--
-- Name: TABLE user_deal_history_86; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_86 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_86 TO matchengine;


--
-- Name: TABLE user_deal_history_87; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_87 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_87 TO matchengine;


--
-- Name: TABLE user_deal_history_88; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_88 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_88 TO matchengine;


--
-- Name: TABLE user_deal_history_89; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_89 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_89 TO matchengine;


--
-- Name: TABLE user_deal_history_9; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_9 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_9 TO matchengine;


--
-- Name: TABLE user_deal_history_90; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_90 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_90 TO matchengine;


--
-- Name: TABLE user_deal_history_91; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_91 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_91 TO matchengine;


--
-- Name: TABLE user_deal_history_92; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_92 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_92 TO matchengine;


--
-- Name: TABLE user_deal_history_93; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_93 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_93 TO matchengine;


--
-- Name: TABLE user_deal_history_94; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_94 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_94 TO matchengine;


--
-- Name: TABLE user_deal_history_95; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_95 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_95 TO matchengine;


--
-- Name: TABLE user_deal_history_96; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_96 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_96 TO matchengine;


--
-- Name: TABLE user_deal_history_97; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_97 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_97 TO matchengine;


--
-- Name: TABLE user_deal_history_98; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_98 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_98 TO matchengine;


--
-- Name: TABLE user_deal_history_99; Type: ACL; Schema: public; Owner: postgres
--

GRANT SELECT ON TABLE public.user_deal_history_99 TO readhistory;
GRANT SELECT,INSERT ON TABLE public.user_deal_history_99 TO matchengine;


--
-- PostgreSQL database dump complete
--

