param (
	[Parameter(Mandatory)]
	[string]$output_path,
	[Parameter(Mandatory)]
	[string]$dependencies_path,
	[Parameter(ValueFromRemainingArguments)]
	[string[]]$dependencies
)

If(!(Test-Path $output_path))
{
	New-Item -ItemType Directory -Force -Path $output_path
}

for ( $i = 0; $i -lt $dependencies.count; $i++ ) {
	$dependency = $dependencies[$i] 
	$dependency_path = Join-Path -Path $dependencies_path -ChildPath $dependency 
	Copy-Item -Path $dependency_path $output_path -Recurse
}