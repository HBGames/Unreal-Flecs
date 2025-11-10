// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "FlecsEntityView.h"

#include "FlecsEntity.generated.h"

#define UE_API FLECSENTITY_API

/** An entity identifier.
 * Entity ids consist out of a number unique to the entity in the lower 32 bits,
 * and a counter used to track entity liveliness in the upper 32 bits. When an
 * id is recycled, its generation count is increased. This causes recycled ids
 * to be very large (>4 billion), which is normal.
 */
typedef flecs::entity_t FFlecsEntityType;

USTRUCT(BlueprintType)
struct UE_API FFlecsEntity : public FFlecsEntityView
{
	GENERATED_BODY()

	using Self = FFlecsEntity;

	FFlecsEntity() = default;

	UE_NODISCARD_CTOR FFlecsEntity(const flecs::entity& InEntity)
	{
		id_ = InEntity.raw_id();
		world_ = InEntity.raw_world();
	}

	/** Create entity.
	 *
	 * @param InWorld The world in which to create the entity.
	 */
	UE_NODISCARD_CTOR explicit FFlecsEntity(flecs::world_t* InWorld)
		: FFlecsEntity(flecs::entity(InWorld))
	{
	}

	/** Wrap an existing entity id.
	 *
	 * @param InWorld The world in which the entity is created.
	 * @param InId The entity id.
	 */
	UE_NODISCARD_CTOR explicit FFlecsEntity(const flecs::world_t* InWorld, const flecs::entity_t InId)
		: FFlecsEntity(flecs::entity(InWorld, InId))
	{
	}

	/** Create a named entity.
	 * Named entities can be looked up with the lookup functions. Entity names
	 * may be scoped, where each element in the name is separated by "::".
	 * For example: "Foo::Bar". If parts of the hierarchy in the scoped name do
	 * not yet exist, they will be automatically created.
	 *
	 * @param InWorld The world in which to create the entity.
	 * @param InName The entity name.
	 */
	UE_NODISCARD_CTOR explicit FFlecsEntity(flecs::world_t* InWorld, const char* InName)
		: FFlecsEntity(flecs::entity(InWorld, InName))
	{
	}

	/** Create a named entity.
	 * Named entities can be looked up with the lookup functions. Entity names
	 * may be scoped, where each element in the name is separated by sep.
	 * For example: "Foo.Bar". If parts of the hierarchy in the scoped name do
	 * not yet exist, they will be automatically created.
	 *
	 * @param InWorld The world in which to create the entity.
	 * @param InName The entity name.
	 * @param InSeparator The separator to use for the scoped name.
	 * @param InRootSeparator The separator to use for the root of the scoped name.
	 */
	UE_NODISCARD_CTOR explicit FFlecsEntity(flecs::world_t* InWorld, const char* InName, const char* InSeparator, const char* InRootSeparator)
		: FFlecsEntity(flecs::entity(InWorld, InName, InSeparator, InRootSeparator))
	{
	}

	/** Conversion from flecs::entity_t to FFlecsEntity.
	 *
	 * @param InId The entity_t value to convert.
	 */
	UE_NODISCARD_CTOR explicit FFlecsEntity(const flecs::entity_t InId)
		: FFlecsEntity(flecs::entity(InId))
	{
	}

	flecs::entity Entity() const { return flecs::entity(world_, id_); }

	/** Get mutable component value.
	 * This operation returns a mutable reference to the component. If the entity
	 * did not yet have the component, it will be added. If a base entity had
	 * the component, it will be overridden, and the value of the base component
	 * will be copied to the entity before this function returns.
	 *
	 * @tparam T The component to get.
	 * @return Reference to the component value.
	 */
	template <typename T>
	T& Obtain() const
	{
		Entity().template obtain<T>();
		return *this;
	}

	/** Add a component to an entity.
	 * To ensure the component is initialized, it should have a constructor.
	 * 
	 * @tparam T the component type to add.
	 */
	template <typename T>
	const Self& Add() const
	{
		Entity().template add<T>();
		return *this;
	}

	/** Add pair for enum constant.
	* This operation will add a pair to the entity where the first element is
	* the enumeration type, and the second element the enumeration constant.
	* 
	* The operation may be used with regular (C style) enumerations as well as
	* enum classes.
	* 
	* @param InValue The enumeration value.
	*/
	template <typename E, flecs::if_t<flecs::is_enum<E>::value> = 0>
	const Self& Add(const E InValue) const
	{
		Entity().template add<E>(InValue);
		return *this;
	}

	/** Add an entity to an entity.
	 * Add an entity to the entity. This is typically used for tagging.
	 *
	 * @param InComponent The component to add.
	 */
	const Self& Add(const flecs::id_t InComponent) const
	{
		Entity().add(InComponent);
		return *this;
	}

	/** Add a pair.
	 * This operation adds a pair to the entity.
	 *
	 * @param InFirst The first element of the pair.
	 * @param InSecond The second element of the pair.
	 */
	const Self& Add(const flecs::entity_t InFirst, const flecs::entity_t InSecond) const
	{
		Entity().add(InFirst, InSecond);
		return *this;
	}

	/** Add a pair.
	 * This operation adds a pair to the entity.
	 *
	 * @tparam First The first element of the pair
	 * @tparam Second The second element of the pair
	 */
	template <typename First, typename Second>
	const Self& Add() const
	{
		Entity().template add<First, Second>();
		return *this;
	}

	/** Add a pair.
	 * This operation adds a pair to the entity.
	 *
	 * @tparam TFirst The first element type of the pair
	 * @tparam TSecond The first element type of the pair
	 * @param InSecond The second element of the pair. 
	 */
	template <typename TFirst, typename TSecond, flecs::if_not_t<flecs::is_enum<TSecond>::value> = 0>
	const Self& Add(TSecond InSecond) const
	{
		Entity().template add<TFirst, TSecond>(InSecond);
		return *this;
	}

	/** Add a pair.
	 * This operation adds a pair to the entity that consists out of a tag
	 * combined with an enum constant.
	 *
	 * @tparam TFirst The first element of the pair
	 * @param InConstant the enum constant.
	 */
	template <typename TFirst, typename TSecond, flecs::if_t<flecs::is_enum<TSecond>::value && !std::is_same<TFirst, TSecond>::value> = 0>
	const Self& Add(const TSecond InConstant) const
	{
		Entity().template add<TFirst, TSecond>(InConstant);
		return *this;
	}

	/** Add a pair.
	 * This operation adds a pair to the entity.
	 *
	 * @param InFirst The first element of the pair
	 * @tparam TSecond The second element of the pair
	 */
	template <typename TSecond>
	const Self& AddSecond(const flecs::entity_t InFirst) const
	{
		Entity().template add_second<TSecond>(InFirst);
		return *this;
	}

	/** Conditional add.
	 * This operation adds if condition is true, removes if condition is false.
	 * 
	 * @param bCondition The condition to evaluate.
	 * @param InComponent The component to add.
	 */
	const Self& AddIf(const bool bCondition, const flecs::id_t InComponent) const
	{
		Entity().add_if(bCondition, InComponent);
		return *this;
	}

	/** Conditional add.
	 * This operation adds if condition is true, removes if condition is false.
	 * 
	 * @tparam T The component to add.
	 * @param bCondition The condition to evaluate.
	 */
	template <typename T>
	const Self& AddIf(const bool bCondition) const
	{
		Entity().template add_if<T>(bCondition);
		return *this;
	}

	/** Conditional add.
	 * This operation adds if condition is true, removes if condition is false.
	 * 
	 * @param bCondition The condition to evaluate.
	 * @param InFirst The first element of the pair.
	 * @param InSecond The second element of the pair.
	 */
	const Self& AddIf(const bool bCondition, const flecs::entity_t InFirst, const flecs::entity_t InSecond) const
	{
		Entity().add_if(bCondition, InFirst, InSecond);
		return *this;
	}

	/** Conditional add.
	 * This operation adds if condition is true, removes if condition is false.
	 * 
	 * @tparam TFirst The first element of the pair
	 * @param bCondition The condition to evaluate.
	 * @param InSecond The second element of the pair.
	 */
	template <typename TFirst>
	const Self& AddIf(const bool bCondition, const flecs::entity_t InSecond) const
	{
		Entity().template add_if<TFirst>(bCondition, InSecond);
		return *this;
	}

	/** Conditional add.
	 * This operation adds if condition is true, removes if condition is false.
	 * 
	 * @tparam TFirst The first element of the pair
	 * @tparam TSecond The second element of the pair
	 * @param bCondition The condition to evaluate.
	 */
	template <typename TFirst, typename TSecond>
	const Self& AddIf(const bool bCondition) const
	{
		Entity().template add_if<TFirst, TSecond>(bCondition);
		return *this;
	}

	/** Conditional add.
	 * This operation adds if condition is true, removes if condition is false.
	 * 
	 * @param bCondition The condition to evaluate.
	 * @param InConstant The enumeration constant.
	 */
	template <typename E, flecs::if_t<flecs::is_enum<E>::value> = 0>
	const Self& AddIf(const bool bCondition, const E InConstant) const
	{
		Entity().template add_if<E>(bCondition, InConstant);
		return *this;
	}

	/** Shortcut for `add(IsA, entity)`.
	 *
	 * @param InSecond The second element of the pair.
	 */
	const Self& IsA(const flecs::entity_t InSecond) const
	{
		Entity().is_a(InSecond);
		return *this;
	}

	/** Shortcut for `add(IsA, entity)`.
	 *
	 * @tparam T the type associated with the entity.
	 */
	template <typename T>
	const Self& IsA() const
	{
		Entity().template is_a<T>();
		return *this;
	}

	/** Shortcut for `add(ChildOf, entity)`.
	 *
	 * @param InSecond The second element of the pair.
	 */
	const Self& ChildOf(const flecs::entity_t InSecond) const
	{
		Entity().child_of(InSecond);
		return *this;
	}

	/** Shortcut for `add(DependsOn, entity)`.
	 *
	 * @param InSecond The second element of the pair.
	 */
	const Self& DependsOn(const flecs::entity_t InSecond) const
	{
		Entity().depends_on(InSecond);
		return *this;
	}


	/** Shortcut for `add(DependsOn, entity)`.
	*
	* @param InSecond The second element of the pair.
	*/
	template <typename E, flecs::if_t<flecs::is_enum<E>::value> = 0>
	const Self& DependsOn(const E InSecond) const
	{
		Entity().template depends_on<E>(InSecond);
		return *this;
	}

	/** Shortcut for `add(SlotOf, entity)`.
	 *
	 * @param InSecond The second element of the pair.
	 */
	const Self& SlotOf(const flecs::entity_t InSecond) const
	{
		Entity().slot_of(InSecond);
		return *this;
	}

	bool ExportTextItem(FString& ValueStr, FFlecsEntity const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const;

protected:
	const Self& ToBase() const
	{
		return *static_cast<const Self*>(this);
	}

public:
	friend uint32 GetTypeHash(const FFlecsEntity& InEntity)
	{
		return GetTypeHash(InEntity.id_);
	}
};

static_assert(std::is_trivially_copyable_v<FFlecsEntity>, "FFlecsEntity must be trivially copyable for BitCast");
static_assert(std::is_trivially_copyable_v<flecs::entity>, "flecs::entity must be trivially copyable for BitCast");
static_assert(sizeof(FFlecsEntity) == sizeof(flecs::entity), "FFlecsEntity and flecs::entity must have same size");
static_assert(alignof(FFlecsEntity) == alignof(flecs::entity), "FFlecsEntity and flecs::entity must have same alignment");

template <>
struct TStructOpsTypeTraits<FFlecsEntity> : public TStructOpsTypeTraitsBase2<FFlecsEntity>
{
	enum
	{
		WithExportTextItem = true,
	};
};

#if !UE_BUILD_DEBUG
#include "FlecsId.inl"
#include "FlecsEntityView.inl"
#include "FlecsEntity.inl"
#endif

#undef UE_API
