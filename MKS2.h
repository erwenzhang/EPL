#include <iostream>
#include <string>
#include <cstdint>


/*
* I want my MKS unit implementation to be more flexible. My original version
* (and Stroustrup's version) suffers from being very fragile and rigid, since
* the code depended on the first exponent being distance (meters), the second
* was always time (seconds), etc. The client side interface wasn't bad because
* we had type aliases created for Meters, Seconds, Kilograms, etc. But, the
* impelementation of MKS was very messy. If you accidentally listed the
* exponents in the wrong order, you'd get incorrect behavior, but it'd be very
* difficult to spot your mistake.
*
* What I want to do is symbolically encode the sequence. One possibility would
* be to use std::tuple and some placeholder types. I create a placeholder type
* (an empty struct, i.e., a "tag") for Distance, Time and Mass. I instantiate a
* std::tuple type with the sequence I want to use, and then I derive all the
* MKS type arithmetic using this sequence:
*
*
#include <tuple>

struct DistancePL {};
struct TimePL {};
struct MassPL {};

using MKSExponentOrder = std::tuple<DistancePL, TimePL, MassPL>;

* That concept *should* work, but std::tuple doesn't provide all the functionality
* that I want. Specifically, I want to take my MKSExponentOrder type alias and
* ask it a question, "what is the position of the TimePL type?"
* The std::tuple doesn't have the functionality to answer this question because
* in a general tuple, there may be multiple fields with the same type, i.e.,
* std::tuple<int, char*, int, double> is a reasonable type. If you asked this
* tuple "what is the position of the int type?" the answer is ambiguous.
* In my application, the "Tuple" I create will have only placeholders in it
* and all the placeholders will be distinct types. So, my question makes perfect
* sense (and can easily be implemented with a recursive metafunction).
* Also, std::tuple isn't very complex, so I don't mind reinventing the wheel
*
* I'll call my version of std::tuple "TypeSequence". In fact, all I really want is
* the type sequence, there won't be any data inside my TypeSequence (unlike tuple
* which is intended to store actual data).
*/

template <typename... Terms>
struct TypeSequence {}; // that's it, it's empty

/* OK, I need to be able to determine the length of the sequence, and to extract
* the index of any of my placeholder types */

template <typename Seq>
struct get_length_TypeSequence; // only valid (and only specialized) when Seq is TypeSequence<...>

/* specialization #1, the recursive general case */
template <typename HeadType, typename... TailTypes>
struct get_length_TypeSequence<TypeSequence<HeadType, TailTypes...>> {
	using Tail = TypeSequence<TailTypes...>;
	static constexpr int tail_length = get_length_TypeSequence<Tail>::length;
	static constexpr int length = 1 + tail_length;
};

/* specialization #2, the base case */
template <typename T>
struct get_length_TypeSequence<TypeSequence<T>> {
	static constexpr int length = 1;
};

/* and a helper function -- this function just gives me a different (and I think better)
* syntax for invoking my get_length meta function, previously, I had
* get_length_TypeSequence<Seq>::length as the only way to invoke the function.
* Now, I can invoke the function with get_length(Seq{});
* Note that Seq must be a TypeSequence<...> and that I'm running the constructor for
* this type to create an argument that is not used by the funciton */
template <typename... Args>
constexpr int get_length(TypeSequence<Args...>) {
	return get_length_TypeSequence<TypeSequence<Args...>>::length;
}

/* I also need to be able to calculate the index pos for a type */
template <typename T, typename Seq>
struct find_TypeSequence; // only valid (and only specialized) when T is TypeSequence<...>

/* specializartion #1, recursive general case */
template <typename T, typename Head, typename... TailTypes>
struct find_TypeSequence<T, TypeSequence<Head, TailTypes...>> {
	static constexpr int tail_position = find_TypeSequence<T, TypeSequence<TailTypes...>>::position;
	static constexpr int position = 1 + tail_position;
};

/* specialization #2, base case */
template <typename T, typename... TailTypes>
struct find_TypeSequence<T, TypeSequence<T, TailTypes...>> {
	static constexpr int position = 0;
};

/* I am not going to attempt to deal with the error case of finding a position
* of a type that doesn't exist, e.g., TypeSequencFind<int, TypeSequence<char, double>>
* instead, I'll let that generate whatever random compiler error I get.
* I'm assuming that I'd have a TypeSequence<void> and there are no TypeSequenceFind
* specializations defined that match, so the compiler will search through the
* TypeSequence<...> until it's exhasusted all the types, and then try to recurse
* into TypeSequenceFind<T, TypeSequence<void>> and fail */

template <typename T, typename... Args>
constexpr int find(T, TypeSequence<Args...>) {
	return find_TypeSequence<T, TypeSequence<Args...>>::position;
}

/* my placeholders, one for each fundamental unit type */
struct DistancePL {};
struct TimePL {};
struct MassPL {};
struct TempPL {};

/* my declaration for the order in which exponents will be recorded */
using MKSExponentSequence = TypeSequence<DistancePL, MassPL, TimePL>;

/* and the constexpr (well, const for VS2013) for the exponent indices for each type */
constexpr int MKSExponentLength = get_length(MKSExponentSequence{});
constexpr int DistanceIndex = find(DistancePL{}, MKSExponentSequence{});
constexpr int TimeIndex = find(TimePL{}, MKSExponentSequence{});
constexpr int MassIndex = find(MassPL{}, MKSExponentSequence{});

/* Finally, I can start creating the ExponentSequence that my MKS units will use */
template <int... > struct ExponentSequence {};

/* Now that I have a flexible and (reasonably) robust way of defining the
* sequence of exponents, I need a way to create the exponent vectors themselves.
*
* Let's start off by creating a way to build a simple sequence <0, 0, 0>
* where the length of that sequence is determined by the length of the
* MKSExponentSequence definition.
*
*/

/* the build_ExponentSequence meta function takes as input an ExponentSequence<j, k>
* (with some number of entries, I'm using two for this example, j and k both integers)
* and returns an ExponentSequence with with ExponentSequence<i, j, k>
* the first argument is i (number of new zeros to add to the sequence, and the second
* argument is base (the partially constructed ExponentSequence
* e.g., ExponentSequence<j, k>
* while the general form of the metafunction does not require base to be an
* ExponentSequence<...> the metafunction will only be valid and specialized when base
* is an ExponentSequence */
template <int i, typename base> struct buildSequence; // only valid (and only specialized) when base is ExponentSequence<...>

/* specialization #1, general recursive case, prepend first to the exponent sequence
* writing the specialization in this way, with three arguments, helps to ensure that
* specialization #2 (base case) is always preferred when we've got just two arguments */
template <int first, int second, int... tail>
struct buildSequence<first, ExponentSequence<second, tail...>> {
	using type = ExponentSequence<first, second, tail...>;
};

/* specialization #2, base case adding the second exponent to a sequence that already
* has one exponent */
template <int first, int second>
struct buildSequence<first, ExponentSequence<second>> {
	using type = ExponentSequence<first, second>;
};

template <int prefix, typename T>
using BuildSequence = typename buildSequence<prefix, T>::type;

/* note that we can use BuildSequence to construct an ExponentSequence that is
* arbitrarily long (we have to invoke BuildSequence each time we add an exponent)
* and with arbitrary exponent values (i.e,. any integer) in the sequence. Keep in mind
* that our initial goal was just to create ExponentSequence<0, 0, 0> automatically,
* the MakeUnitlessExponent meta function gets down to business performing that task */

/* this is the general form of the metafunction. In this case, I'm providing an implementation
* for the general template (i.e., not specialized). */
template <int length>
struct make_UnitlessExponent {
	using TailType = typename make_UnitlessExponent<length - 1>::type;
	using type = typename BuildSequence<0, TailType>;
};

template <> struct make_UnitlessExponent<1> {
	using type = ExponentSequence<0>;
};

template <int length>
using MakeUnitlessExponent = typename make_UnitlessExponent<length>::type;

/* OK, so MakeExponentSequence<3> will give me ExponentSequence<0, 0, 0>
* Now, I want to be able to create DistanceSequence where
* the vectors is <1, 0, 0> or <0, 1, 0> (whatever) depending
* on where the DistancePL type is in the ExponentSequence type alias
* To get there, I'll write a metafunction to increment an index
* in an ExponentSequence, then I can simply increment the correct
* index (by using TypeSequenceFind to identify the index).
*/

template <int index, typename Seq>
struct increment_ExponentIndex; // only valid (and only specialized) when Seq is ExponentSequence<...>

/* the general case is kinda a nuisance. There might be more clever ways to proceed, but
* what I'm going to do is the following:
* given ExponentSequence<a, b, c> and assuming I want to increment the second creating
* ExponenentSequence<a, b+1, c>,
* I'll first remove the first exponent: ExponentSequence<b, c>
* then I'll increment the first index (using the base case below): ExponentSequence<b+1,c>
* and finally I'll reattach the leading index: ExponentSequence<a, b+1,c>
*/
template <int index, int head, int... tail>
struct increment_ExponentIndex<index, ExponentSequence<head, tail...>> {
	using stripped = ExponentSequence<tail...>;
	using incremented = typename increment_ExponentIndex<index - 1, stripped>::type;
	using type = BuildSequence<head, incremented>;
};

template <int head, int... tail>
struct increment_ExponentIndex<0, ExponentSequence<head, tail...>> {
	using type = ExponentSequence<head + 1, tail...>;
};

template <int index, typename T>
using IncrementExponentIndex = typename increment_ExponentIndex<index, T>::type;

using MKSUnitless = MakeUnitlessExponent<MKSExponentLength>;

/* finally, I can create the basic exponent types for distanct, time and mass without having
* to remember whether Distance is <1, 0, 0> or <0, 1, 0> */

using MKSDistance = IncrementExponentIndex<DistanceIndex, MKSUnitless>;
using MKSTime = IncrementExponentIndex<TimeIndex, MKSUnitless>;
using MKSMass = IncrementExponentIndex<MassIndex, MKSUnitless>;

inline std::string MKSRawUnit(MKSDistance) { return "distance"; }
inline std::string MKSRawUnit(MKSTime) { return "time"; }
inline std::string MKSRawUnit(MKSMass) { return "mass"; }

/* OK, so there's a few unit vectors for us, now we need the ability to add and subtract
* these vectors (multiplying units means we add exponent vectors), dividing units means
* we'll subtract vectors) */

template <typename E1, typename E2>
struct addExponents; // only valid (and only specialized) when T1 and T2 are both ExponentSequence<...>

template <int h1, int h2, int... tail1, int... tail2>
struct addExponents<ExponentSequence<h1, tail1...>, ExponentSequence<h2, tail2...>> {
	using TailType = typename addExponents<ExponentSequence<tail1...>, ExponentSequence<tail2...>>::type;
	using type = typename BuildSequence<h1 + h2, TailType>;
};

template <int h1, int h2>
struct addExponents<ExponentSequence<h1>, ExponentSequence<h2>> {
	using type = ExponentSequence<h1 + h2>;
};

template <typename E1, typename E2>
using AddExponents = typename addExponents<E1, E2>::type;

template <typename T1, typename T2>
struct subExponents; // only valid (and only specialized) when T1 and T2 are both ExponentSequence<...>

template <int h1, int h2, int... tail1, int... tail2>
struct subExponents<ExponentSequence<h1, tail1...>, ExponentSequence<h2, tail2...>> {
	using TailType = typename subExponents<ExponentSequence<tail1...>, ExponentSequence<tail2...>>::type;
	using type = typename BuildSequence<h1 - h2, TailType>;
};

template <int h1, int h2>
struct subExponents<ExponentSequence<h1>, ExponentSequence<h2>> {
	using type = ExponentSequence<h1 - h2>;
};

template <typename E1, typename E2>
using SubExponents = typename subExponents<E1, E2>::type;

/* and now we can create all the exponent vectors we'll need */

using MKSVelocity = SubExponents<MKSDistance, MKSTime>; // e.g. meters per second
using MKSAcceleration = SubExponents<MKSVelocity, MKSTime>; // e.g., meters per second-squared
using MKSForce = AddExponents<MKSMass, MKSAcceleration>; // e.g., newton or kg*meters/second-squared
using MKSWork = AddExponents<MKSDistance, MKSForce>; // e.g., joule, or kg*meters-squared/second-squared

/* note that if we were to add another unit type to our exponents (e.g., Kelvin), then
* all the typedefs above are still correct. We just update the type alias for
* MKSExponentSequence to be TypeSequence<DistancePL,TimePL,MassPL,TemperaturePL> and
* everything should still work. Similarly, we could change the sequence
* (MKS is, after all, supposed to be Meters, Kilograms, Seconds)
* using MKSExponentSequence = TypeSequence<DistancePL, MassPL, TimePL>
*
* you have to admit that's pretty cool, no?
*/

/* Next on the todo list is to deal with the scaling, things like grams/kilograms
* meters/millimeters, etc. and also meters vs. feet
* The plan for that is to represent the scaling as rational numbers
* Every MKS unit in our system will have a scale associated to it. The meaning of the
* scale is as follows:
* if x is an MKS unit with scale s, then to convert x into the standard units for
* it's type, we need to multiply x by s.
* So, if x is millimeters, then s will be 1/1000
*
* Looking down the road a bit, the actual MKS type will be a template triple
* One template argument will be the ExponentSequence<...>, one template argument
* will be the Scale<num,den> and the third will be the type argument that tells us
* how the actual value is stored (float, double, int, complex<>, etc.).
* It is important to note that the pair of ExponentSequence + Scale determines the
* symbolic names for the units, i.e., Meters vs Inches vs Pounds. So, when we get ready
* to start doing I/O we'll want to keep that in mind.
*
* When we're dealing with scales, we should probably keep in mind that the same rational
* number can be expressed in multiple ways, e.g., 1/2 is the same as 5/10. That's a bit of
* a nuisance. Let's try tackling the following problem.
* Let's create a list of standard scales, e.g., for distance measures
* using DistanceList = < <<1,1>,Meters>, <<1000,1>,Kilometers>, <<100,3937>,Inches>, <<1200,3937>,Feet>>
* that awkward structure is a list of pairs. Each pair is a scale and a standard distance measure
* We'd like to be able to search this list with a scale, such as <1000,39370> and return the matching type (Inches).
* Ideally, when searching the list, if we have a scale that evaluates to an equivalent fraction, it still matches
* Finaly, if we don't find a match, we'll return void
* whew! that seems like a good challenge. Let's start off with a simple question
* Given two rationals, are these two rationals equivalent (we'll evaluate to a boolean true/false).
*/

template <uint64_t num, uint64_t den>
struct Rational {};

template <typename T1, typename T2>
struct EquivRationals; // only valid when T1 and T2 are Rational<...>

template <uint64_t n1, uint64_t d1, uint64_t n2, uint64_t d2>
struct EquivRationals<Rational<n1, d1>, Rational<n2, d2>> {
	static const bool value = (n1 * d2 == n2 * d1);
};


template <typename S1, typename S2> struct multiplyScales;
template <uint64_t n1, uint64_t d1, uint64_t n2, uint64_t d2>
struct multiplyScales<Rational<n1, d1>, Rational<n2, d2>> {
	using type = Rational<n1*n2, d1*d2>;
};
template <typename S1, typename S2> using MultiplyScales = typename multiplyScales<S1, S2>::type;

template <typename S1, typename S2> struct divideScales;
template <uint64_t n1, uint64_t d1, uint64_t n2, uint64_t d2>
struct divideScales<Rational<n1, d1>, Rational<n2, d2>> {
	using type = Rational<n1*d2, d1*n2>;
};

template <typename S1, typename S2> using DivideScales = typename divideScales<S1, S2>::type;


template <typename Rat, typename ExpSeq>
struct Unit; // only valid when Rat is a Rational and ExpSeq is an ExponentSequence

template <uint64_t num, uint64_t den, int... exponents>
struct Unit<Rational<num, den>, ExponentSequence<exponents...>> {
	using Scale = Rational<num, den>;
	using Exponents = ExponentSequence<exponents...>;

	template <uint64_t scale>
	using times = Unit<Rational<num * scale, den>, ExponentSequence<exponents...>>;

	template <uint64_t scale>
	using divided = Unit<Rational<num, den * scale>, ExponentSequence<exponents...>>;
};

template <uint64_t num, uint64_t den, int... exponents>
inline const std::string MKSsuffix(Unit<Rational<num, den>, ExponentSequence<exponents...>>) {
	std::ostringstream out;
	out << "*" << num << "/" << "den" << MKSTypeName();
	return "*"
}

template <typename T>
const std::string MKSSuffix(void) { return "unknown MKS unit"; }

using MeterUnit = Unit<Rational<1, 1>, MKSDistance>;
template <> const std::string MKSSuffix<MeterUnit>(void) { return "m"; }
using KilometerUnit = MeterUnit::times<1000>;
template<> const std::string MKSSuffix<KilometerUnit>(void) { return "km"; }
using MillimeterUnit = MeterUnit::divided<1000>;
template<> const std::string MKSSuffix<MillimeterUnit>(void) { return "mm"; }
using InchUnit = MeterUnit::times<100>::divided<3937>;
template<> const std::string MKSSuffix<InchUnit>(void) { return "in"; }
using FootUnit = InchUnit::times<12>;
template<> const std::string MKSSuffix<FootUnit>(void) { return "ft"; }
using MileUnit = FootUnit::times<5280>;
template<> const std::string MKSSuffix<MileUnit>(void) { return "mile"; }

using SecondUnit = Unit<Rational<1, 1>, MKSTime>;
template<> const std::string MKSSuffix<SecondUnit>(void) { return "s"; }
using MillisecondUnit = SecondUnit::divided<1000>;
template<> const std::string MKSSuffix<MillisecondUnit>(void) { return "ms"; }
using MicrosecondUnit = MillisecondUnit::divided<1000>;
template<> const std::string MKSSuffix<MicrosecondUnit>(void) { return "us"; }
using NanosecondUnit = MicrosecondUnit::divided<1000>;
template<> const std::string MKSSuffix<NanosecondUnit>(void) { return "ns"; }
using HourUnit = SecondUnit::times<3600>;
template<> const std::string MKSSuffix<HourUnit>(void) { return "h"; }

using KilogramUnit = Unit<Rational<1, 1>, MKSMass>;
template<> const std::string MKSSuffix<KilogramUnit>(void) { return "kg"; }
using GramUnit = KilogramUnit::divided<1000>;
template<> const std::string MKSSuffix<GramUnit>(void) { return "g"; }


template <typename... T>
struct List {

};

using StandardDistances = List<MeterUnit, KilometerUnit, MillimeterUnit, InchUnit, FootUnit, MileUnit>;
using StandardTimes = List<SecondUnit, MillisecondUnit, MicrosecondUnit, NanosecondUnit>;
using StandardMasses = List<KilogramUnit, GramUnit>;

using StandardUnitsMap =
List<std::pair<MKSDistance, StandardDistances>,
std::pair<MKSTime, StandardTimes>,
std::pair<MKSMass, StandardMasses>
>;

template <typename Rat, typename... List>
struct FindUnit; // only valid when Rat is a Rational and List is a UnitList

template <bool p, typename T, typename F> struct if_then_else;
template <typename T, typename F> struct if_then_else<true, T, F> { using type = T; };
template <typename T, typename F> struct if_then_else<false, T, F> { using type = F; };

template <bool p, typename T, typename F>
using IfThen = typename if_then_else<p, T, F>::type;

template <typename Rat, typename Head, typename... Tail>
struct FindUnit<Rat, Head, Tail...> {
	using type = IfThen<EquivRationals<Rat, typename Head::Scale>::value,
	Head, typename FindUnit<Rat, Tail...>::type>;
};

template <typename Rat, typename Head>
struct FindUnit<Rat, Head> {
	using type = IfThen < EquivRationals<Rat, typename Head::Scale>::value,
	Head, void>;
};

template <typename T>
struct extractExponentFromUnit;
template <typename T>
struct extractScaleFromUnit;

template <typename S, typename E>
struct extractExponentFromUnit<Unit<S, E>> {
	using type = E;
};

template <typename S, typename E>
struct extractScaleFromUnit<Unit<S, E>> {
	using type = S;
};

template <typename T>
using ExtractExponentFromUnit = typename extractExponentFromUnit<T>::type;

template <typename T>
using ExtractScaleFromUnit = typename extractScaleFromUnit<T>::type;


template <typename U1, typename U2>
struct UnitsDifferOnlyByScale;

template <typename R1, typename R2, typename E1, typename E2>
struct UnitsDifferOnlyByScale<Unit<R1, E1>, Unit<R2, E2>> {
	static const bool value = false;
};

template <typename R1, typename R2, typename E>
struct UnitsDifferOnlyByScale<Unit<R1, E>, Unit<R2, E>> {
	static const bool value = true;
};

template <typename U1, typename U2>
struct unitProduct;

template <typename E1, typename E2, typename S1, typename S2>
struct unitProduct<Unit<S1, E1>, Unit<S2, E2>> {
	using Exponent = AddExponents<E1, E2>;
	using Scale = MultiplyScales<S1, S2>;

	using type = Unit<Scale, Exponent>;
};
template <typename U1, typename U2> using UnitProduct = typename unitProduct<U1, U2>::type;

template <typename U1, typename U2>
struct unitQuotient;

template <typename E1, typename E2, typename S1, typename S2>
struct unitQuotient<Unit<S1, E1>, Unit<S2, E2>> {
	using Exponent = SubExponents<E1, E2>;
	using Scale = DivideScales<S1, S2>;

	using type = Unit<Scale, Exponent>;
};
template <typename U1, typename U2> using UnitQuotient = typename unitQuotient<U1, U2>::type;


template <typename T>
struct scale_helper;

template <int num, int den>
struct scale_helper<Rational<num, den>> {
	template <typename T>
	static T apply(T const& value) {
		return (value * num) / den;
	}

	template <typename T>
	static T un_apply(T const& value) {
		return (value * den) / num;
	}
};

template <typename R, typename T>
constexpr T scale_by(T const& value) {
	return scale_helper<R>::apply(value);
}

template <typename R, typename T>
constexpr T unscale_by(T const& value) {
	return scale_helper<R>::un_apply(value);
}


template <typename T, typename U>
class MKSType {
	using Same = MKSType<T, U>;
	using Exponent = ExtractExponentFromUnit<U>;
	using Scale = ExtractScaleFromUnit<U>;

	template <typename T2, typename U2>
	friend class MKSType;
public:
	explicit MKSType(const T& v) : value{ v } {}
	MKSType(const Same&) = default;

	template <typename U2, typename = std::enable_if<UnitsDifferOnlyByScale<U, U2>::value, void>::type>
	MKSType(const MKSType<T, U2>& rhs) {
		using MyScale = ExtractScaleFromUnit<U>;
		using TheirScale = ExtractScaleFromUnit<U2>;
		/* first scale rhs units to the standard scale */
		value = scale_by<TheirScale>(rhs.value);
		/* then scale from standard scale to whatever scale we're using */
		value = unscale_by<MyScale>(value);
	}

	Same& operator=(const Same&) = default;



	void print(std::ostream& out) const {
		out << value << MKSSuffix<U>();
	}

	Same& operator+=(const Same& rhs) {
		value += rhs.value;
		return *this;
	}

	T value;
private:
	template <typename U1, typename U2>
	friend MKSType <T, UnitProduct<U1, U2>> operator*(const MKSType<T, U1>&, const MKSType<T, U2>&);

	template <typename U1, typename U2>
	friend MKSType <T, UnitQuotient<U1, U2>> operator/(const MKSType<T, U1>&, const MKSType<T, U2>&);
};

template <typename T, typename Unit>
std::ostream& operator<<(std::ostream& out, MKSType<T, Unit>& x) {
	x.print(out);
	return out;
}
template <typename T, typename Unit1, typename Unit2>
typename
std::enable_if<UnitsDifferOnlyByScale<Unit1, Unit2>::value, MKSType<T, Unit1>>::type
operator+(MKSType<T, Unit1> x, const MKSType<T, Unit2>& y) {
	x += MKSType<T, Unit1>{y};
	return x;
}

template <typename T, typename Unit1, typename Unit2>
MKSType<T, UnitProduct<Unit1, Unit2>>
operator*(const MKSType<T, Unit1>& x, const MKSType<T, Unit2>& y) {
	return MKSType<T, UnitProduct<Unit1, Unit2>>{x.value*y.value};
}

template <typename T, typename Unit1, typename Unit2>
MKSType<T, UnitQuotient<Unit1, Unit2>>
operator/(const MKSType<T, Unit1>& x, const MKSType<T, Unit2>& y) {
	return MKSType<T, UnitQuotient<Unit1, Unit2>>{x.value / y.value};
}

template <typename T>
using Meters = MKSType<T, MeterUnit>;

template <typename T>
using Inches = MKSType<T, InchUnit>;

template <typename T>
using Kilometers = MKSType<T, KilogramUnit>;

template <typename T>
using Seconds = MKSType<T, SecondUnit>;

template <typename T>
using Milliseconds = MKSType<T, MillisecondUnit>;

template <typename T>
using Grams = MKSType<T, GramUnit>;

template <typename T>
using Kilograms = MKSType<T, KilogramUnit>;

using MeterPerSecondUnit = UnitQuotient<MeterUnit, SecondUnit>;
template<> const std::string MKSSuffix<MeterPerSecondUnit>(void) { return "mps"; }
template <typename T>
using MetersPerSecond = MKSType<T, MeterPerSecondUnit>;

using KPHUnit = UnitQuotient<KilometerUnit, HourUnit>;
template<> const std::string MKSSuffix<KPHUnit>(void) { return "kph"; }
template <typename T>
using KPH = MKSType<T, KPHUnit>;

using MPHUnit = UnitQuotient<MileUnit, HourUnit>;
template<> const std::string MKSSuffix<MPHUnit>(void) { return "mph"; }
template <typename T>
using MPH = MKSType<T, MPHUnit>;

