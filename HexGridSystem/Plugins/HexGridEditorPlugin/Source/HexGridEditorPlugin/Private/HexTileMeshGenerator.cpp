// Copyright 2022 Marc Ohlmann, All Rights Reserved.


#include "HexTileMeshGenerator.h"

#include "HexGridEditorPlugin.h"
#include "MCommon.h"

#include "Engine/StaticMesh.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "Components/DynamicMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"

#include "GeometryScript/MeshAssetFunctions.h"
#include "GeometryScript/CollisionFunctions.h"
#include "AssetToolsModule.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorAssetLibrary.h"
#include "FileHelpers.h"


AHexTileMeshGenerator::AHexTileMeshGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	TileRadius = 50.f;
	HexTileMaterial = nullptr;
	_GeometryScriptDebug = nullptr;
	GenerateStaticMesh = false;

	DynamicMeshComp = CreateDefaultSubobject<UDynamicMeshComponent>("DynamicMeshComp");
	RootComponent = DynamicMeshComp;
}


UDynamicMesh* AHexTileMeshGenerator::GenerateHexTileDynamicMesh()
{
	IF_NULL_RETURN_NULL_LERR(DynamicMeshComp, HexGridEditor, "could not get dynamic mesh component");

	UDynamicMesh* HexTileDynamicMesh = DynamicMeshComp->GetDynamicMesh();
	IF_NULL_RETURN_NULL_LERR(HexTileDynamicMesh, HexGridEditor, "could not get dynamic mesh");

	// construct dynamic mesh
	HexTileDynamicMesh->Reset();

	RenderDynamicMeshHexagon(HexTileDynamicMesh, GetTileRadius(), 0);

	DynamicMeshComp->SetDynamicMesh(HexTileDynamicMesh);

	// apply material
	if (UMCommon::IsUObjectPointerSafe(HexTileMaterial))
	{
		DynamicMeshComp->SetMaterial(0, HexTileMaterial);
	}


	return HexTileDynamicMesh;
}


void AHexTileMeshGenerator::PrintGeometryScriptDebugMessages() const
{
	IF_NULL_RETURN_LERR(_GeometryScriptDebug, HexGridEditor, "geometry script debug object is null");

	FOR_FORWARD(i, _GeometryScriptDebug->Messages.Num())
	{
		L_ERR(HexGridEditor, "{'{0}', '{1}'}: {2}",
			_GeometryScriptDebug->Messages[i].ErrorType,
			_GeometryScriptDebug->Messages[i].MessageType,
			_GeometryScriptDebug->Messages[i].Message
		);
	}


	return;
}


void AHexTileMeshGenerator::RenderDynamicMeshHexagon(UDynamicMesh * TargetMesh, const float HexTileRadius, const int GroupId)
{
	IF_NULL_RETURN_LERR(TargetMesh, HexGridEditor, "could not get dynamic mesh");

	FDynamicMesh3* MeshData = TargetMesh->GetMeshPtr();
	if (!MeshData)
	{
		L_ERR(HexGridEditor, "could not get dynamic mesh data");
		return;
	}

	const int CenterVertexId = MeshData->AppendVertex(
		UE::Geometry::FVertexInfo(
			FHexGridCoordinate::c_Zero.GetCenterWorldOffsetLocation(HexTileRadius),
			FVector3f::UpVector
		)
	);
	TArray<FVector> CornerVertices = FHexGridCoordinate::c_Zero.GetCornerWorldOffsetLocations(HexTileRadius);
	TArray<int> CornerVerticeIds;
	FOR_FORWARD(i, CornerVertices.Num())
	{
		CornerVerticeIds.Add(
			MeshData->AppendVertex(
				UE::Geometry::FVertexInfo(
					CornerVertices[i]
				)
			)
		);
	}

	// top face triangles
	MeshData->AppendTriangle(CornerVerticeIds[1], CenterVertexId, CornerVerticeIds[0], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[2], CenterVertexId, CornerVerticeIds[1], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[3], CenterVertexId, CornerVerticeIds[2], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[4], CenterVertexId, CornerVerticeIds[3], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[5], CenterVertexId, CornerVerticeIds[4], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[0], CenterVertexId, CornerVerticeIds[5], GroupId);

	// side face triangles
	const FVector HeighOffset = FVector::DownVector * UMathTypes::c_Half * HexTileRadius;
	TArray<int> HeightOffsetCornerVerticeIds;
	FOR_FORWARD(i, CornerVertices.Num())
	{
		HeightOffsetCornerVerticeIds.Add(
			MeshData->AppendVertex(
				UE::Geometry::FVertexInfo(
					CornerVertices[i] + HeighOffset
				)
			)
		);
	}

	MeshData->AppendTriangle(CornerVerticeIds[0], HeightOffsetCornerVerticeIds[0], HeightOffsetCornerVerticeIds[1], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[1], CornerVerticeIds[0], HeightOffsetCornerVerticeIds[1], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[1], HeightOffsetCornerVerticeIds[1], HeightOffsetCornerVerticeIds[2], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[2], CornerVerticeIds[1], HeightOffsetCornerVerticeIds[2], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[2], HeightOffsetCornerVerticeIds[2], HeightOffsetCornerVerticeIds[3], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[3], CornerVerticeIds[2], HeightOffsetCornerVerticeIds[3], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[3], HeightOffsetCornerVerticeIds[3], HeightOffsetCornerVerticeIds[4], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[4], CornerVerticeIds[3], HeightOffsetCornerVerticeIds[4], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[4], HeightOffsetCornerVerticeIds[4], HeightOffsetCornerVerticeIds[5], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[5], CornerVerticeIds[4], HeightOffsetCornerVerticeIds[5], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[5], HeightOffsetCornerVerticeIds[5], HeightOffsetCornerVerticeIds[0], GroupId);
	MeshData->AppendTriangle(CornerVerticeIds[0], CornerVerticeIds[5], HeightOffsetCornerVerticeIds[0], GroupId);


	return;
}


void AHexTileMeshGenerator::GenerateHexTileStaticMesh()
{
	// make unique name / path for package / asset
	const FName AssetName = FName("HexTileStaticMesh");
	FString PackageName = FString(TEXT("/Game/")) + AssetName.ToString();
	FString FinalAssetName;
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(PackageName, TEXT(""), PackageName, FinalAssetName);

	// create modal window to prompt the user for a name for the new mesh
	TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
		SNew(SDlgPickAssetPath)
		.Title(NSLOCTEXT("HexGridMeshGen", "ConvertToStaticMeshPickName", "Choose New StaticMesh Location"))
		.DefaultAssetPath(FText::FromString(PackageName));
	if (PickAssetPathWidget->ShowModal() == EAppReturnType::Ok)
	{
		// Get the full name of where we want to create the physics asset.
		FString UserPackageName = PickAssetPathWidget->GetFullAssetPath().ToString();
		FName MeshName(*FPackageName::GetLongPackageAssetName(UserPackageName));

		// Check if the user inputed a valid asset name, if they did not, give it the generated default name
		if (MeshName == NAME_None)
		{
			// Use the defaults that were already generated.
			UserPackageName = PackageName;
			MeshName = *FinalAssetName;
		}

		// create package
		UPackage* Package = CreatePackage(*UserPackageName);
		IF_NULL_RETURN_LERR(Package, HexGridEditor, "failed to create package for hex tile static mesh");

		// create a static mesh to copy data into as part of the package
		UStaticMesh* NewHexTileStaticMesh = NewObject<UStaticMesh>(Package, MeshName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
		IF_NULL_RETURN_LERR(NewHexTileStaticMesh, HexGridEditor, "failed to create hex tile static mesh");

		// create the dynamic mesh
		UDynamicMesh* HexTileDynamicMesh = GenerateHexTileDynamicMesh();
		IF_NULL_RETURN_LERR(HexTileDynamicMesh, HexGridEditor, "could not get dynamic mesh");

		// create debug object if it does not already exist
		if (UMCommon::IsUObjectPointerSafe(_GeometryScriptDebug) == false)
		{
			_GeometryScriptDebug = NewObject<UGeometryScriptDebug>(this);
		}

		// copy the dynamic mesh into the transient static mesh object
		FGeometryScriptCopyMeshToAssetOptions CopyAssetOptions = FGeometryScriptCopyMeshToAssetOptions();
		FGeometryScriptMeshWriteLOD WriteLOD = FGeometryScriptMeshWriteLOD();
		EGeometryScriptOutcomePins Outcome;
		UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshToStaticMesh(
			HexTileDynamicMesh,
			NewHexTileStaticMesh,
			CopyAssetOptions,
			WriteLOD,
			Outcome,
			true,
			_GeometryScriptDebug
		);

		// check CopyMeshToStaticMesh outcome for errors
		const EGeometryScriptOutcomePins ResultingOutcome = Outcome;
		switch (ResultingOutcome)
		{

		case EGeometryScriptOutcomePins::Success:
			L_VERBOSE(HexGridEditor, "succeeded at creating hex tile static mesh: '{0}'", NewHexTileStaticMesh);
			break; // continue to saving asset

		case EGeometryScriptOutcomePins::Failure:
			L_ERR(HexGridEditor, "failed to generate hex tile static mesh");
			PrintGeometryScriptDebugMessages();
			return; // exit on error

		default:
			L_ERR(HexGridEditor, "default on CopyMeshToStaticMesh outcome: '{0}'", ResultingOutcome);
			return; // exit on error
		}

		// use the same mesh as collison
		FGeometryScriptCollisionFromMeshOptions CollisionFromMeshOptions;
		CollisionFromMeshOptions.Method = EGeometryScriptCollisionGenerationMethod::Capsules;
		FGeometryScriptSetStaticMeshCollisionOptions SetStaticMeshCollisionOptions;
		UGeometryScriptLibrary_CollisionFunctions::SetStaticMeshCollisionFromMesh(
			HexTileDynamicMesh,
			NewHexTileStaticMesh,
			CollisionFromMeshOptions,
			SetStaticMeshCollisionOptions,
			_GeometryScriptDebug
		);
		PrintGeometryScriptDebugMessages();

		//
		// save the copied static mesh as a new asset
		//

		// register asset???
		FAssetRegistryModule::AssetCreated(Package);
		//FAssetRegistryModule::AssetCreated(NewHexTileStaticMesh);

		L_VERBOSE(HexGridEditor, "Mesh = '{0}', Package = '{1}', UserPackageName = '{2}', FinalAssetName = '{3}'", 
			NewHexTileStaticMesh, 
			Package, 
			UserPackageName,
			FinalAssetName
		);

		// save the asset
		Package->MarkPackageDirty();
		UEditorAssetLibrary::SaveAsset(UserPackageName, true);
	}
	else
	{
		L_WARN(HexGridEditor, "canceled creating static mesh");
	}


	return;
}


void AHexTileMeshGenerator::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject) == true &&
		GenerateStaticMesh == true && 
		PropertyChangedEvent.GetPropertyName().IsEqual(FName("GenerateStaticMesh")) && 
		PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet)
	{
		GenerateStaticMesh = false;
		GenerateHexTileStaticMesh();
	}


	return;
}

